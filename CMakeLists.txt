# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (FOD-C)
set(CMAKE_BUILD_TYPE "Debug")

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -fsigned-char")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3 -std=c++11")

set(ROOT_DIR ${CMAKE_SOURCE_DIR})
set(SRC_DIR ${CMAKE_SOURCE_DIR}/FOD-C)

include_directories("${ROOT_DIR}/3rd/Include")

file(GLOB_RECURSE SRC "${SRC_DIR}/*.cpp" "${SRC_DIR}/*.cc")

add_executable(${PROJECT_NAME}  ${SRC})

find_library(crypto, /usr/local/openssl/lib/)

target_link_libraries(${PROJECT_NAME}  libprotobuf.a libprotobuf-lite.a libuv.a /usr/local/openssl/lib/libcrypto.a pthread ${CMAKE_DL_LIBS})

