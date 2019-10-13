cmake_minimum_required(VERSION 3.13)
project(guiness-native-api)

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

if(NOT DEFINED ENV{JAVA_HOME})
    message("java home not found, please edit manually cmake line 63 for specify your java home directory")
    set(JAVA_HOME "C:/Program Files (x86)/Java/jdk1.8.0_221")
endif()

find_package(JNI REQUIRED)
include_directories(${JNI_INCLUDE_DIRS})
include_directories(include)

add_library(guinness-native-api MODULE src/library.cpp ${HADES_SRC} ${BEA_SOURCES} ${ASMJIT_SOURCES} src/keysend.cpp)
#add_executable(guinness-native-api src/library.cpp ${HADES_SRC} ${BEA_SOURCES} ${ASMJIT_SOURCES} src/keysend.cpp)

target_link_libraries(guinness-native-api CONAN_PKG::boost user32 advapi32 dbghelp shell32 psapi imagehlp ole32 ${JNI_LIBRARIES})


if ("${CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE}" STREQUAL "x86")
    message("guinness-native-api will be compiled for 32bits processes")
    set_target_properties(guinness-native-api PROPERTIES SUFFIX "-x32.dll")
else()
    message("guinness-native-api will be compiled for 64bits processes")
    set_target_properties(guinness-native-api PROPERTIES SUFFIX "-x64.dll")
endif()