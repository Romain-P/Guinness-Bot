#ifndef PACKET_INTERCEPTOR_LIBRARY_H
#define PACKET_INTERCEPTOR_LIBRARY_H

#include "Interceptor.h"
#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD_PTR ul_reason_for_call, LPVOID lpReserved);
DWORD WINAPI onLibraryLoaded(HMODULE module);
LRESULT CALLBACK WindowHook(int nCode, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK createWindow(HWND window, LPARAM processId);
int MessageBoxCentered(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);

#endif