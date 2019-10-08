#include <iostream>

#include <HadesMemory/Injector.hpp>
#include <HadesMemory/MemoryMgr.hpp>
#include <AsmJit/Assembler.h>

using HadesMem::MemoryMgr;
using HadesMem::Injector;

int main(int ac, char **args) {
    MemoryMgr const memory(9812);
    Injector const injector(memory);

    injector.InjectDll(L"winsock-patcher.dll", HadesMem::Injector::InjectFlag_PathResolution);
    return 0;
}