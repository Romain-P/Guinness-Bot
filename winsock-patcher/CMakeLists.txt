cmake_minimum_required(VERSION 3.13)
project(winsock-patcher)

set(CMAKE_CXX_STANDARD 11)

include_directories(lib/minhook)

add_library(winsock-patcher MODULE src/library.cpp src/library.h src/Interceptor.cpp src/Interceptor.h src/Hook.h src/ModuleHideHelpers.cpp src/ModuleHideHelpers.h)

if ("${CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE}" STREQUAL "x86")
    message("winsock-patcher will be compiled for 32bits processes")
    target_link_libraries(winsock-patcher ${CMAKE_SOURCE_DIR}/lib/minhook/libMinHook-x86-v141-md.lib)
else()
    message("winsock-patcher will be compiled for 64bits processes")
    target_link_libraries(winsock-patcher ${CMAKE_SOURCE_DIR}/lib/minhook/libMinHook-x64-v141-md.lib)
endif()

message("winsock-patcher is ready to compile, open the .svn solution located in /build")