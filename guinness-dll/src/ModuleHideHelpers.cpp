#include "ModuleHideHelpers.h"

std::vector<UNLINKED_MODULE> UnlinkedModules;

void RelinkModuleToPEB(HMODULE hModule)
{
    std::vector<UNLINKED_MODULE>::iterator it = std::find_if(UnlinkedModules.begin(), UnlinkedModules.end(), FindModuleHandle(hModule));

    if (it == UnlinkedModules.end())
    {
        //DBGOUT(TEXT("Module Not Unlinked Yet!"));
        return;
    }
    UNLINKED_MODULE m = *it;
    RELINK(m.Entry->InLoadOrderModuleList, m.RealInLoadOrderLinks);
    RELINK(m.Entry->InInitializationOrderModuleList, m.RealInInitializationOrderLinks);
    RELINK(m.Entry->InMemoryOrderModuleList, m.RealInMemoryOrderLinks);
    UnlinkedModules.erase(it);
}

void UnlinkModuleFromPEB(HMODULE hModule)
{
    std::vector<UNLINKED_MODULE>::iterator it = std::find_if(UnlinkedModules.begin(), UnlinkedModules.end(), FindModuleHandle(hModule));
    if (it != UnlinkedModules.end())
    {
        //DBGOUT(TEXT("Module Already Unlinked!"));
        return;
    }

#ifdef _WIN64
    PPEB pPEB = (PPEB)__readgsqword(0x60);
#else
    PPEB pPEB = (PPEB)__readfsdword(0x30);
#endif
    PLIST_ENTRY CurrentEntry = pPEB->Ldr->InLoadOrderModuleList.Flink;
    PLDR_MODULE Current = NULL;
    while (CurrentEntry != &pPEB->Ldr->InLoadOrderModuleList && CurrentEntry != NULL)
    {
        Current = CONTAINING_RECORD(CurrentEntry, LDR_MODULE, InLoadOrderModuleList);
        if (Current->BaseAddress == hModule)
        {
            UNLINKED_MODULE CurrentModule = { 0 };
            CurrentModule.hModule = hModule;
            CurrentModule.RealInLoadOrderLinks = Current->InLoadOrderModuleList.Blink->Flink;
            CurrentModule.RealInInitializationOrderLinks = Current->InInitializationOrderModuleList.Blink->Flink;
            CurrentModule.RealInMemoryOrderLinks = Current->InMemoryOrderModuleList.Blink->Flink;
            CurrentModule.Entry = Current;
            UnlinkedModules.push_back(CurrentModule);

            UNLINK(Current->InLoadOrderModuleList);
            UNLINK(Current->InInitializationOrderModuleList);
            UNLINK(Current->InMemoryOrderModuleList);

            break;
        }

        CurrentEntry = CurrentEntry->Flink;
    }
}


void RemovePeHeader(HANDLE GetModuleBase)
{
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)GetModuleBase;
    PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((PBYTE)pDosHeader + (DWORD)pDosHeader->e_lfanew);

    if (pNTHeader->Signature != IMAGE_NT_SIGNATURE)
        return;

    if (pNTHeader->FileHeader.SizeOfOptionalHeader)
    {
        DWORD Protect;
        WORD Size = pNTHeader->FileHeader.SizeOfOptionalHeader;
        VirtualProtect((void*)GetModuleBase, Size, PAGE_EXECUTE_READWRITE, &Protect);
        SecureZeroMemory((void*)GetModuleBase, Size);
        VirtualProtect((void*)GetModuleBase, Size, Protect, &Protect);
    }
}