//
// Created by romain on 10/12/2019.
//

#include <string>
#include "keysend.h"
#include <tlhelp32.h>

HWND handle = nullptr;

BOOL CALLBACK onWindowFound(HWND windowHandle, LPARAM callBackParam) {
    DWORD processId;
    GetWindowThreadProcessId(windowHandle, &processId);

    if (processId == callBackParam) {
        handle = windowHandle;
        return FALSE;
    }
    return TRUE;
}

void sendKeyStroke(HWND windowHandle, WPARAM key) {
    PostMessage(windowHandle, WM_KEYDOWN, key, 0);
    PostMessage(windowHandle, WM_KEYUP, key, 0);
}

HWND findWindowHandle(DWORD processId) {
    handle = nullptr;
    EnumWindows(onWindowFound, processId);
    return handle;
}

HWND findWindowHandle(std::wstring const &processName) {
    DWORD processId = 0;
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry) == TRUE){
        while (Process32Next(snapshot, &entry) == TRUE) {
            if ((wcscmp((wchar_t *) entry.szExeFile, processName.c_str()) == 0)) {
                processId = entry.th32ProcessID;
                break;
            }
        }
    }
    CloseHandle(snapshot);

    return processId != 0 ? findWindowHandle(processId) : nullptr;
}

void sendUtf16String(HWND windowHandle, std::wstring const &str) {
    for (wchar_t c: str)
        PostMessageW(windowHandle, WM_CHAR, (WPARAM) c, 0);
}
