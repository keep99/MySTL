cmake_minimum_required(VERSION 3.17)

add_definitions(-std=c++11)

set(Perform_Src perform_main.cpp ../Profiler/profiler.cpp ../src/alloc.cpp)
add_executable(stl_perform ${Perform_Src})

include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/Profiler")

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")