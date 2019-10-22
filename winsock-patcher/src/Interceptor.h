#ifndef PACKET_INTERCEPTOR_INTERCEPTOR_H
#define PACKET_INTERCEPTOR_INTERCEPTOR_H

#include <ws2tcpip.h>
#include <string>
#include <vector>
#include "MinHook.h"

using connect_t = int (WINAPI *)(SOCKET,const sockaddr *name, int length);
using recv_t = int (WINAPI *)(SOCKET, char *buffer, int len, int flags);
int WINAPI onConnect(SOCKET,const sockaddr *name, int length);
int WINAPI onRecv(SOCKET, char *buffer, int len, int flags);

class Interceptor {
public:
    static constexpr char *localhost = "127.0.0.1";
    static UINT32 magic_offset;
    static UINT32 proxy_port;
    static constexpr UINT16 ankama_auth_port = 443;
    static constexpr char *game_packet_header = ".{GH}.";
    static constexpr char *auth_packet_header = ".{AH}.";
    static constexpr char *auth_game_ticket_header = ".{AGTH}.";
    static constexpr char *game_select_packet_header = "AXK";
    static constexpr char ankama_packet_delimiter = 0x0;
    static constexpr char client_packet_delimiter[2] = {0xa, 0x0};

    void config(HMODULE module);
    void init();
    void exit();
    void uncompressGameIP(char const *data, SIZE_T len);
    std::string filePath(std::string const &filename);

    UINT16 const &gamePort() const;
    std::string const &gameIp() const;
    bool &listenPackets();
    std::string const &gameTicket() const;
    void resetGameInformations();
    std::vector<char> &globalBuffer();
private:
    bool _listenPackets;
    std::string _gameIp;
    UINT16 _gamePort;
    std::string _gameTicket;
    std::string _dllPath;
    std::vector<char> _globalBuffer;
};

#endif //PACKET_INTERCEPTOR_INTERCEPTOR_H
