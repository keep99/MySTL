cmake_minimum_required(VERSION 3.17)

add_definitions(-std=c++11)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

# set(SRC_LIST main.cpp)
# add_executable(main ${SRC_LIST})