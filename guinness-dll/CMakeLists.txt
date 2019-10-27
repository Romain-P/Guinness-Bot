cmake_minimum_required(VERSION 3.13)
project(guinness-native-patcher)

set(CMAKE_CXX_STANDARD 11)

include_directories(lib/minhook)

add_library(guinness-native-patcher MODULE src/Library.cpp src/Library.h src/Interceptor.cpp src/Interceptor.h src/Hook.h src/ModuleHideHelpers.cpp src/ModuleHideHelpers.h)

if ("${CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE}" STREQUAL "x86")
    message("guinness-native-patcher will be compiled for 32bits processes")
    target_link_libraries(guinness-native-patcher ${CMAKE_SOURCE_DIR}/lib/minhook/libMinHook-x86-v141-md.lib)
    set_target_properties(guinness-native-patcher PROPERTIES SUFFIX "-x32.dll")
else()
    message("guinness-native-patcher will be compiled for 64bits processes")
    target_link_libraries(guinness-native-patcher ${CMAKE_SOURCE_DIR}/lib/minhook/libMinHook-x64-v141-md.lib)
    set_target_properties(guinness-native-patcher PROPERTIES SUFFIX "-x64.dll")
endif()

target_link_libraries(guinness-native-patcher wsock32 ws2_32)
message("guinness-native-patcher is ready to compile, open the .svn solution located in /build")