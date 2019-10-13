#ifndef PACKET_INTERCEPTOR_INTERCEPTOR_H
#define PACKET_INTERCEPTOR_INTERCEPTOR_H

#include <ws2tcpip.h>
#include <string>
#include "MinHook.h"

using connect_t = int (WINAPI *)(SOCKET,const sockaddr *name, int length);
int WINAPI onConnect(SOCKET,const sockaddr *name, int length);

class Interceptor {
public:
    static constexpr char *localhost = "127.0.0.1";
    static constexpr char *proxy_port_filename = ".proxy-port";
    static constexpr UINT16 ankama_auth_port = 443;

    void loadProxyPort();
    void config(HMODULE module);
    void init();
    void exit();
    void debug();
    std::string filePath(std::string const &filename);

    UINT16 proxyPort() { return _proxyPort; }
    void saveAnkamaHost(struct in_addr *ptr);

private:
    UINT16 _proxyPort;
    std::string _dllPath;
};

#endif //PACKET_INTERCEPTOR_INTERCEPTOR_H
