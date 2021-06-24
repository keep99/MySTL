cmake_minimum_required(VERSION 3.17)

project(MYSTL)

add_definitions(-std=c++11)  # 增加对 C++11 的支持
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

add_subdirectory(src)
add_subdirectory(thirdparty/gtest)
enable_testing()
add_subdirectory(Test)
add_subdirectory(Profiler)
add_subdirectory(Performance)