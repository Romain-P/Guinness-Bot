#ifndef PACKET_INTERCEPTOR_HIDEMODULE_H
#define PACKET_INTERCEPTOR_HIDEMODULE_H

#include <vector>
#include <windows.h>
#include <algorithm>

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWCH   Buffer;
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;

typedef struct _PEB_LDR_DATA {
    ULONG      Length;
    BOOLEAN    Initialized;
    PVOID      SsHandle;
    LIST_ENTRY InLoadOrderModuleList;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;


typedef struct _PEB {
#ifdef _WIN64
    UINT8 _PADDING_[24];
#else
    UINT8 _PADDING_[12];
#endif
    PEB_LDR_DATA* Ldr;
} PEB, *PPEB;
typedef struct _LDR_MODULE
{
    LIST_ENTRY      InLoadOrderModuleList;
    LIST_ENTRY      InMemoryOrderModuleList;
    LIST_ENTRY      InInitializationOrderModuleList;
    PVOID           BaseAddress;
    PVOID           EntryPoint;
    ULONG           SizeOfImage;
    UNICODE_STRING  FullDllName;
    UNICODE_STRING  BaseDllName;
    ULONG           Flags;
    SHORT           LoadCount;
    SHORT           TlsIndex;
    LIST_ENTRY      HashTableEntry;
    ULONG           TimeDateStamp;
} LDR_MODULE, *PLDR_MODULE;

typedef struct _UNLINKED_MODULE
{
    HMODULE hModule;
    PLIST_ENTRY RealInLoadOrderLinks;
    PLIST_ENTRY RealInMemoryOrderLinks;
    PLIST_ENTRY RealInInitializationOrderLinks;
    PLDR_MODULE Entry; // =PLDR_DATA_TABLE_ENTRY
} UNLINKED_MODULE;

#define UNLINK(x)					\
	(x).Flink->Blink = (x).Blink;	\
	(x).Blink->Flink = (x).Flink;

#define RELINK(x, real)			\
	(x).Flink->Blink = (real);	\
	(x).Blink->Flink = (real);	\
	(real)->Blink = (x).Blink;	\
	(real)->Flink = (x).Flink;

struct FindModuleHandle
{
    HMODULE m_hModule;
    FindModuleHandle(HMODULE hModule) : m_hModule(hModule)
    {
    }
    bool operator() (UNLINKED_MODULE const &Module) const
    {
        return (Module.hModule == m_hModule);
    }
};

void UnlinkModuleFromPEB(HMODULE hModule);
void RelinkModuleToPEB(HMODULE hModule);
void RemovePeHeader(HANDLE GetModuleBase);

#endif //PACKET_INTERCEPTOR_HIDEMODULE_H
