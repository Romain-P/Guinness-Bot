#include "Interceptor.h"
#include "Hook.h"
#include <fcntl.h>
#include <stdio.h>
#include <io.h>
#include <string>
#include <fstream>
#include <commdlg.h>
#include <iostream>
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

    int result = winsockRecv(socket, buffer, len, flags);

    if (result <= 0 || !ctx.listenPackets() || sourcePort != Interceptor::proxy_port || strcmp(sourceIp, Interceptor::localhost) != 0)
        return result;

    /** append buffer **/
    ctx.globalBuffer().insert(ctx.globalBuffer().end(), buffer, buffer + result);

    std::vector<char>::iterator it;
    while(!ctx.globalBuffer().empty() && (it = std::find(ctx.globalBuffer().begin(), ctx.globalBuffer().end(), Interceptor::ankama_packet_delimiter)) != ctx.globalBuffer().end()) {
        std::string packet(ctx.globalBuffer().begin(), it);

        /** check if the server list packet is found **/
        if (packet.rfind(Interceptor::game_select_packet_header, 0) != std::string::npos) {
            std::string data = packet.substr(strlen(Interceptor::game_select_packet_header));
            /** catch game port and ip **/
            ctx.uncompressGameIP(data.c_str(), data.size());
            std::string ticketPacket = Interceptor::auth_game_ticket_header + ctx.gameTicket();
            std::vector<char> packetBuff;
            packetBuff.insert(packetBuff.end(), ticketPacket.begin(), ticketPacket.end());
            packetBuff.insert(packetBuff.end(), Interceptor::client_packet_delimiter, Interceptor::client_packet_delimiter + sizeof(Interceptor::client_packet_delimiter));
            send(socket, &packetBuff[0], packetBuff.size(), 0);
        }

        /** next packet **/
        ctx.globalBuffer().erase(ctx.globalBuffer().begin(), it + 1);
    }

    return result;
}

int WINAPI onConnect(SOCKET socket, const sockaddr *addr, int length) {
    auto sock = (struct sockaddr_in *) ((sockaddr *) addr);
    auto targetPort = ntohs(sock->sin_port);
    char targetIp[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &(sock->sin_addr), targetIp, INET_ADDRSTRLEN);

    std::string packetHeader;
    bool gameConnect = false;

    /** if the connection is targeting a game server **/
    if (!ctx.gameIp().empty() && ctx.gameIp() == targetIp && targetPort == ctx.gamePort()) {
        gameConnect = true;
        ctx.listenPackets() = false;
        packetHeader = Interceptor::game_packet_header;
    }
    /** if the connection is targeting the auth server **/
    else if (targetPort == Interceptor::ankama_auth_port) {
        ctx.listenPackets() = true;
        packetHeader = Interceptor::auth_packet_header;
    }
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
    std::string packet = packetHeader + targetIp + ':' + std::to_string(targetPort);
    if (gameConnect) {
        packet += ':' + ctx.gameTicket();
        ctx.resetGameInformations();
    }
    std::vector<char> buffer;
    buffer.insert(buffer.end(), packet.begin(), packet.end());
    buffer.insert(buffer.end(), Interceptor::client_packet_delimiter, Interceptor::client_packet_delimiter + sizeof(Interceptor::client_packet_delimiter));
    send(socket, &buffer[0], buffer.size(), 0);

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

bool &Interceptor::listenPackets() {
    return _listenPackets;
}

std::string const &Interceptor::gameTicket() const {
    return _gameTicket;
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
    _gameTicket = wrap.substr(11);
}

std::vector<char> &Interceptor::globalBuffer() {
    return _globalBuffer;
}

void Interceptor::resetGameInformations() {
    _gameIp.clear();
    _gamePort = 0;
    _gameTicket.clear();
}