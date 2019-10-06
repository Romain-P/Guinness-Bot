#include <iostream>
#include "Injector.h"

int main() {
    Injector injector;
    std::cout << injector.find_target_process("Dofus") << std::endl;
    std::cout << injector.inject("winsock-patcher.dll") << std::endl;
    return 0;
}