# CMake 最低版本号要求
cmake_minimum_required(VERSION 3.0)

project(test_cxx_coroutine LANGUAGES C CXX)

# C++版本
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# C版本
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_C_STANDARD_REQUIRED ON)

# GCC 10要求：Coroutine必须要设置该标识，否则不能使用。
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")


string(TOUPPER "${CMAKE_BUILD_TYPE}" BUILD_TYPE)

message(STATUS "[cmake] Build for OS type:      ${CMAKE_SYSTEM_NAME}")
message(STATUS "[cmake] Build for OS version:   ${CMAKE_SYSTEM_VERSION}")
message(STATUS "[cmake] Build for CPU type:     ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "[cmake] Build type:             ${CMAKE_BUILD_TYPE}")
message(STATUS "[cmake] C++ Compiler path:      ${CMAKE_CXX_COMPILER}")
message(STATUS "[cmake] C++ Compiler type:      ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "[cmake] C++ Compiler version:   ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "[cmake] C Compiler path:        ${CMAKE_C_COMPILER}")
message(STATUS "[cmake] C Compiler type:        ${CMAKE_C_COMPILER_ID}")
message(STATUS "[cmake] C Compiler version:     ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "[cmake] Build with cxx flags:   ${CMAKE_CXX_FLAGS_${BUILD_TYPE}} ${CMAKE_CXX_FLAGS}")
message(STATUS "[cmake] Build with c flags:     ${CMAKE_C_FLAGS_${BUILD_TYPE}} ${CMAKE_C_FLAGS}")

add_subdirectory(base)
add_subdirectory(fibonacci)
add_subdirectory(promise)
