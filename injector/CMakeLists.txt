cmake_minimum_required(VERSION 3.13)
project(injector)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)

add_compile_definitions(BOOST_FILESYSTEM_VERSION=3)
add_compile_definitions(BOOST_SYSTEM_NO_DEPRECATED)
add_compile_definitions(BOOST_FILESYSTEM_NO_DEPRECATED)
add_compile_definitions(UNICODE)
add_compile_definitions(_UNICODE)
add_compile_definitions(STRICT)
add_compile_definitions(WIN32_LEAN_AND_MEAN)
add_compile_definitions(WINVER=_WIN32_WINNT_WS03)
add_compile_definitions(HADES_MSVC)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)
INCLUDE_DIRECTORIES(${CONAN_INCLUDE_DIRS})

include_directories(src/AsmJit)
include_directories(src/BeatEngine/include)
include_directories(src/HadesMemory/include)

add_compile_definitions(ASMJIT_API=)

# AsmJit C++ sources
Set(ASMJIT_SOURCES
        src/AsmJit/AsmJit/Assembler.cpp
        src/AsmJit/AsmJit/AssemblerX86X64.cpp
        src/AsmJit/AsmJit/CodeGenerator.cpp
        src/AsmJit/AsmJit/Compiler.cpp
        src/AsmJit/AsmJit/CompilerX86X64.cpp
        src/AsmJit/AsmJit/CpuInfo.cpp
        src/AsmJit/AsmJit/Defs.cpp
        src/AsmJit/AsmJit/DefsX86X64.cpp
        src/AsmJit/AsmJit/Logger.cpp
        src/AsmJit/AsmJit/MemoryManager.cpp
        src/AsmJit/AsmJit/Operand.cpp
        src/AsmJit/AsmJit/OperandX86X64.cpp
        src/AsmJit/AsmJit/Platform.cpp
        src/AsmJit/AsmJit/Util.cpp
        )

Include_Directories(src)
INCLUDE_DIRECTORIES(src/AsmJit)

add_compile_definitions(BEA_ENGINE_STATIC)
set (BEA_SOURCES src/BeatEngine/BeaEngine.c)
include_directories(src/BeatEngine/include)

file(GLOB HADES_SRC
        src/HadesMemory/*.cpp
        src/HadesMemory/PeLib/*.cpp
        src/HadesMemory/Detail/*.cpp
        src/HadesMemory/PeLib/Detail/*.cpp
        src/HadesMemory/Experimental/*.cpp)

include_directories(src/HadesMemory/include)

add_executable(injector src/main.cpp ${HADES_SRC} ${BEA_SOURCES} ${ASMJIT_SOURCES})

target_link_libraries(injector CONAN_PKG::boost user32 advapi32 dbghelp shell32 psapi imagehlp ole32)