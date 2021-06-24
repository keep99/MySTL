cmake_minimum_required(VERSION 3.17)

add_definitions(-std=c++11)

set(Test_Src test_main.cpp ../src/alloc.cpp test_helper.cpp)
add_executable(stl_test ${Test_Src})

include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${gmock_SOURCE_DIR}/include ${gmock_SOURCE_DIR})

target_link_libraries(stl_test gtest gtest_main)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")