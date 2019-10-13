#include "Interceptor.h"
#include "Hook.h"
#include <fcntl.h>
#include <stdio.h>
#include <io.h>
#include <string>
#include <fstream>
#include <commdlg.h>
#include <iostream>

Interceptor ctx;
connect_t winsockConnect = nullptr;

void Interceptor::loadProxyPort() {
    std::ifstream ifs(filePath(Interceptor::proxy_port_filename));
    std::string content((std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>()));

    _proxyPort = (UINT16) std::atoi(content.c_str());
}

void Interceptor::init() {
    //debug();
    loadProxyPort();
    MH_Initialize();
    MH_CreateHookApi(L"Ws2_32", "connect", (LPVOID) onConnect, (LPVOID *) &winsockConnect);
    MH_EnableHook(MH_ALL_HOOKS);
}

int WINAPI onConnect(SOCKET socket, const sockaddr *addr, int length) {
    auto sock = (struct sockaddr_in *) ((sockaddr *) addr);
    auto targetPort = ntohs(sock->sin_port);

    if (targetPort == Interceptor::ankama_auth_port) {
        ctx.saveAnkamaHost(&sock->sin_addr);
        InetPton(AF_INET, Interceptor::localhost, &(sock->sin_addr));
        sock->sin_port = htons(ctx.proxyPort());
    }

    return winsockConnect(socket, addr, length);
}

void Interceptor::exit() {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}

void Interceptor::debug() {
    if (!AllocConsole()) {
        char buffer[1024] = {0};
        sprintf_s(buffer, "Failed to AllocConsole( ), GetLastError( ) = %d", GetLastError());
        MessageBox(HWND_DESKTOP, buffer, "Error", MB_OK);
        return;
    }

    auto lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    auto hConHandle = _open_osfhandle(PtrToUlong(lStdHandle), _O_TEXT);
    auto fp = _fdopen(hConHandle, "w");

    freopen_s(&fp, "CONOUT$", "w", stdout);
    *stdout = *fp;
    setvbuf(stdout, NULL, _IONBF, 0);
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

void Interceptor::saveAnkamaHost(struct in_addr *ptr) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, ptr, ip, INET_ADDRSTRLEN);

    std::ofstream outfile(".ankama-host");
    outfile << ip;
}
