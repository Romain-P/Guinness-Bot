cmake_minimum_required(VERSION 3.13)
project(packet_interceptor)

set(CMAKE_CXX_STANDARD 11)

include_directories(lib/minhook)

add_library(packet_interceptor SHARED src/library.cpp src/library.h)
target_link_libraries(packet_interceptor ${CMAKE_SOURCE_DIR}/lib/minhook/libMinHook-x86-v141-md.lib)