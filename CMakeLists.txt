cmake_minimum_required(VERSION 3.17)

PROJECT(MYSTL)

add_definitions(-std=c++11)  # 增加对 C++11 的支持
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")


ADD_SUBDIRECTORY(src bin)
ADD_SUBDIRECTORY(Test)