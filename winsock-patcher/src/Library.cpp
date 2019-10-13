#include "Library.h"
#include "Hook.h"
#include "ModuleHideHelpers.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD_PTR ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            ctx.config(hModule);
            DisableThreadLibraryCalls(hModule);
            CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE) onLibraryLoaded, hModule, 0, nullptr);

            /**
             * Unlink dll from PEB & remove heads <=> invisible module
             */
            UnlinkModuleFromPEB(hModule);
            RemovePeHeader(hModule);
            break;
        case DLL_PROCESS_DETACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            ctx.exit();
            break;
    }
    return TRUE;
}

HHOOK hHookCBT{};

LRESULT CALLBACK WindowHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HCBT_CREATEWND) {
        if (((LPCBT_CREATEWND) lParam)->lpcs->lpszClass == (LPSTR) 32770) {
            RECT rcParent{};
            GetWindowRect(((LPCBT_CREATEWND) lParam)->lpcs->hwndParent, &rcParent);

            ((LPCBT_CREATEWND) lParam)->lpcs->x =
                    rcParent.left + ((rcParent.right - rcParent.left) - ((LPCBT_CREATEWND) lParam)->lpcs->cx) / 2;
            ((LPCBT_CREATEWND) lParam)->lpcs->y =
                    rcParent.top + ((rcParent.bottom - rcParent.top) - ((LPCBT_CREATEWND) lParam)->lpcs->cy) / 2;
        }
    }

    return CallNextHookEx(hHookCBT, nCode, wParam, lParam);
}

int MessageBoxCentered(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType) {
    hHookCBT = SetWindowsHookExA(WH_CBT, WindowHook, nullptr, GetCurrentThreadId());

    int iRet{MessageBox(hWnd, lpText, lpCaption, uType)};
    UnhookWindowsHookEx(hHookCBT);

    return iRet;
}


BOOL CALLBACK createWindow(HWND window, LPARAM processId) {
    DWORD winProcessId;
    GetWindowThreadProcessId(window, &winProcessId);

    if (winProcessId == processId) {
        SetWindowTextA(window, "Guinness");
        //MessageBoxCentered(window, "Login and enjoy your guinness", "Now drink a guinness ", MB_APPLMODAL);
        return FALSE;
    }
    return TRUE;
}

DWORD WINAPI onLibraryLoaded(HMODULE module) {
    ctx.init();
    EnumWindows(createWindow, (LPARAM) GetCurrentProcessId());
    return 0;
}