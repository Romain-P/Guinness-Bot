#include "MinHook.h"
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

void Interceptor::init() {
    debug();
    MH_Initialize();
    MH_CreateHookApi(L"Ws2_32", "connect", (LPVOID) onConnect, (LPVOID *) &winsockConnect);
    MH_EnableHook(MH_ALL_HOOKS);
}

int WINAPI onConnect(SOCKET socket, const sockaddr *name, int length) {
    std::cout << "hello from dll" << std::endl;
    return winsockConnect(socket, name, length);
}

void Interceptor::exit() {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}

void Interceptor::debug() {
    if (!AllocConsole()) {
        char buffer[1024] = { 0 };
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