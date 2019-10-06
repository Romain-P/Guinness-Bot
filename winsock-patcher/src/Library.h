#ifndef PACKET_INTERCEPTOR_LIBRARY_H
#define PACKET_INTERCEPTOR_LIBRARY_H

#include "Interceptor.h"
#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD_PTR ul_reason_for_call, LPVOID lpReserved);
DWORD WINAPI onLibraryLoaded(HMODULE module);

#endif