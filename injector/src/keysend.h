//
// Created by romain on 10/12/2019.
//

#ifndef GUINESS_NATIVE_API_KEYSEND_H
#define GUINESS_NATIVE_API_KEYSEND_H

#include <windows.h>

extern HWND handle;

BOOL CALLBACK onWindowFound(HWND windowHandle, LPARAM callBackParam);
HWND findWindowHandle(DWORD processId);
HWND findWindowHandle(std::wstring const &processName);
void sendKeyStroke(HWND windowHandle, WPARAM key);
void sendUtf16String(HWND windowHandle, std::wstring const &str);



#endif //GUINESS_NATIVE_API_KEYSEND_H
