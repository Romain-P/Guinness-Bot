#include "Interceptor.h"
#include "Hook.h"
#include <fcntl.h>
#include <stdio.h>
#include <io.h>
#include <string>
#include <fstream>
#include <commdlg.h>
#include <iostream>
#include <vector>
#include <sstream>

Interceptor ctx;
connect_t winsockConnect = nullptr;
recv_t winsockRecv = nullptr;
UINT32 Interceptor::magic_offset = 0xcafebabe;
UINT32 Interceptor::proxy_port = 0xdeadbeaf;

void Interceptor::init() {
    MH_Initialize();
    MH_CreateHookApi(L"Ws2_32", "connect", (LPVOID) onConnect, (LPVOID *) &winsockConnect);
    MH_CreateHookApi(L"Ws2_32", "recv", (LPVOID) onRecv, (LPVOID *) &winsockRecv);
    MH_EnableHook(MH_ALL_HOOKS);
}

int WINAPI onRecv(SOCKET socket, char *buffer, int len, int flags) {
    SOCKADDR_IN sock = {0};
    int size = sizeof(sock);
    getpeername(socket, (sockaddr*)&sock, &size);
    char sourceIp[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(sock.sin_addr), sourceIp, INET_ADDRSTRLEN);
    auto sourcePort = ntohs(sock.sin_port);

    /** we only intercept auth packets **/
    if (sourcePort != Interceptor::ankama_auth_port || ctx.gameIp() == sourceIp)
        return winsockRecv(socket, buffer, len, flags);

    /** append buffer **/
    ctx.globalBuffer().insert(ctx.globalBuffer().end(), buffer, buffer + len);
    SIZE_T pos;

    /** check if a several valid packets are found **/
    while ((pos = ctx.globalBuffer().find('\n')) != std::string::npos) {
        std::string packet = ctx.globalBuffer().substr(0, pos);

        /** check if the server list packet is found **/
        if (packet.rfind(Interceptor::game_select_packet_header, 0) != std::string::npos) {
            std::string data = packet.substr(strlen(Interceptor::game_select_packet_header));
            /** catch game port and ip **/
            ctx.uncompressGameIP(data.c_str(), data.size());
        }

        /** next packet **/
        ctx.globalBuffer() = ctx.globalBuffer().substr(pos + 1, ctx.globalBuffer().size());
    }

    return winsockRecv(socket, buffer, len, flags);
}

int WINAPI onConnect(SOCKET socket, const sockaddr *addr, int length) {
    auto sock = (struct sockaddr_in *) ((sockaddr *) addr);
    auto targetPort = ntohs(sock->sin_port);
    char targetIp[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &(sock->sin_addr), targetIp, INET_ADDRSTRLEN);

    std::string packetHeader;

    /** if the connection is targeting a game server **/
    if (ctx.gameIp() == targetIp && targetPort == ctx.gamePort())
        packetHeader = Interceptor::game_packet_header;
    /** if the connection is targeting the auth server **/
    else if (targetPort == Interceptor::ankama_auth_port)
        packetHeader = Interceptor::auth_packet_header;
    /** any other port/ip are skipped **/
    else
        return winsockConnect(socket, addr, length);

    /** reset global buffer (ensure there is no corrupted memory) **/
    ctx.globalBuffer().clear();

    /** redirect connect to the MITM **/
    InetPton(AF_INET, Interceptor::localhost, &(sock->sin_addr));
    sock->sin_port = htons((UINT16) Interceptor::proxy_port);

    /** connect **/
    int result = winsockConnect(socket, addr, length);

    /** send game/auth host to mitm
     *
     * AUTH: .{AI}.ip:port
     * GAME: .{GI}.ip:port
     *
     * e.g .{AI}.0.0.0.0:443  => Ankama auth server (ip = 0.0.0.0 | port = 443)
     */
    std::stringstream ss;
    ss << packetHeader << targetIp << ':' << targetPort << '\n';
    std::string packet = ss.str();
    send(socket, packet.c_str(), packet.size(), 0);

    return result;
}

void Interceptor::exit() {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}

void Interceptor::config(HMODULE module) {
    char path[MAX_PATH];
    GetModuleFileNameA(module, path, sizeof(path));
    _dllPath = std::string(path);

    size_t i = _dllPath.rfind('\\', _dllPath.length());
    _dllPath = _dllPath.substr(0, i);
}

std::string Interceptor::filePath(std::string const &filename) {
    return _dllPath + "\\" + filename;
}

UINT16 const &Interceptor::gamePort() const {
    return _gamePort;
}

std::string const &Interceptor::gameIp() const {
    return _gameIp;
}

std::string &Interceptor::authIp() {
    return _authIp;
}

void Interceptor::uncompressGameIP(char const *data, SIZE_T len) {
    static std::string BASE = {
            'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
            'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D',
            'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S',
            'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '_'
    };

    std::string wrap(data, data + len);
    std::string ipPart = wrap.substr(0, 8);
    std::string portPart = wrap.substr(8, 3);
    std::string ticketId = wrap.substr(11);
    std::vector<int> ipBytes;

    for (size_t i = 0; i < 8; i += 2) {
        char lastBits = ipPart.at(i) - 48;
        char firstBits = ipPart.at(i + 1) - 48;
        ipBytes.push_back(((lastBits & 15) << 4 | firstBits & 15));
    }

    std::stringstream ss;
    for(size_t i = 0; i < ipBytes.size(); ++i) {
        if(i != 0)
            ss << ".";
        ss << ipBytes[i];
    }

    _gamePort = (BASE.find(portPart[0]) & 63) << 12 | (BASE.find(portPart[1]) & 63) << 6 | (BASE.find(portPart[2]) & 63);
    _gameIp = ss.str();
}

std::string &Interceptor::globalBuffer() {
    return _globalBuffer;
}
