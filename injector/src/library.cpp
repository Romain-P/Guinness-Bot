#include <iostream>

#include <HadesMemory/Injector.hpp>
#include <HadesMemory/MemoryMgr.hpp>
#include <AsmJit/Assembler.h>
#include <jni.h>
#include "com_guiness_bot_external_NativeAPI.h"
#include <tlhelp32.h>
#include <cstdio>
#include <string>

using HadesMem::MemoryMgr;
using HadesMem::Injector;

JNIEXPORT void JNICALL Java_com_guiness_bot_external_NativeAPI_inject(JNIEnv *env, jobject obj, jint processId, jstring dllPath) {
    auto id = (DWORD) processId;
    MemoryMgr const memory(id);
    Injector const injector(memory);

    unsigned char isCopy = 1;
    std::string a((*env).GetStringUTFChars(dllPath, &isCopy));
    std::wstring b(a.begin(), a.end());
    injector.InjectDll(b, HadesMem::Injector::InjectFlag_PathResolution);
}

JNIEXPORT void JNICALL Java_com_guiness_bot_external_NativeAPI_injectDofus(JNIEnv *env, jobject obj, jstring dllPath) {
    DWORD processId = 0;
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry) == TRUE){
        while (Process32Next(snapshot, &entry) == TRUE) {
            if ((wcscmp((wchar_t *) entry.szExeFile, L"Dofus.exe") == 0)) {
                processId = entry.th32ProcessID;
                break;
            }
        }
    }
    CloseHandle(snapshot);
    if (processId)
        Java_com_guiness_bot_external_NativeAPI_inject(env, obj, processId, dllPath);
}