#ifndef PACKET_INTERCEPTOR_INTERCEPTOR_H
#define PACKET_INTERCEPTOR_INTERCEPTOR_H

using connect_t = int (WINAPI *)(SOCKET,const sockaddr *name, int length);
int WINAPI onConnect(SOCKET,const sockaddr *name, int length);

class Interceptor {
public:
    void init();
    void exit();
    void debug();

private:
    static constexpr UINT32 PROXY_PORT = 0x346B5DB0;
};

#endif //PACKET_INTERCEPTOR_INTERCEPTOR_H
