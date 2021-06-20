cmake_minimum_required(VERSION 3.17)

add_definitions(-std=c++11)


SET(SRC_LIST main.cpp)
ADD_EXECUTABLE(main ${SRC_LIST})

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")