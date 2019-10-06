#include "library.h"
#include "Interceptor.h"
#include "Hook.h"
#include "ModuleHideHelpers.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD_PTR ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
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

DWORD WINAPI onLibraryLoaded(HMODULE module) {
    ctx.init();
    while (true)
    {
        Sleep(10);
    }
    return 0;
}