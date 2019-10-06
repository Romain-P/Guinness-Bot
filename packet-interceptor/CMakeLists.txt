cmake_minimum_required(VERSION 3.13)
project(packet_interceptor)

set(CMAKE_CXX_STANDARD 11)

include_directories(lib/minhook)

add_library(packet_interceptor MODULE src/library.cpp src/library.h src/Interceptor.cpp src/Interceptor.h src/Hook.h src/ModuleHideHelpers.cpp src/ModuleHideHelpers.h)

if ("${CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE}" STREQUAL "x86")
    message("packet-interceptor will be compiled for 32bits processes")
    target_link_libraries(packet_interceptor ${CMAKE_SOURCE_DIR}/lib/minhook/libMinHook-x86-v141-md.lib)
else()
    message("packet-interceptor will be compiled for 64bits processes")
    target_link_libraries(packet_interceptor ${CMAKE_SOURCE_DIR}/lib/minhook/libMinHook-x64-v141-md.lib)
endif()

message("packet-interceptor is ready to compile, open the .svn solution located in /build")