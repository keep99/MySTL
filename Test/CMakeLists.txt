cmake_minimum_required(VERSION 3.17)

add_definitions(-std=c++11)

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/src")

SET(Vector_Test_Src vector_test.cpp ../src/alloc.cpp)
ADD_EXECUTABLE(vector_test ${Vector_Test_Src})

SET(List_Test_Src list_test.cpp ../src/alloc.cpp)
ADD_EXECUTABLE(list_test ${List_Test_Src})

SET(Deque_Test_Src deque_test.cpp ../src/alloc.cpp)
ADD_EXECUTABLE(deque_test ${Deque_Test_Src})

SET(Algo_Test_Src algo_test.cpp ../src/alloc.cpp)
ADD_EXECUTABLE(algo_test ${Algo_Test_Src})

SET(Set_Test_Src set_test.cpp ../src/alloc.cpp)
ADD_EXECUTABLE(set_test ${Set_Test_Src})

SET(Map_Test_Src map_test.cpp ../src/alloc.cpp)
ADD_EXECUTABLE(map_test ${Map_Test_Src})

SET(Rb_tree_Test_Src rb_tree_test.cpp ../src/alloc.cpp)
ADD_EXECUTABLE(rb_tree_test ${Rb_tree_Test_Src})

SET(Unordered_map_Test_Src unordered_map_test.cpp ../src/alloc.cpp)
ADD_EXECUTABLE(unordered_map_test ${Unordered_map_Test_Src})

SET(Unordered_set_Test_Src unordered_set_test.cpp ../src/alloc.cpp)
ADD_EXECUTABLE(unordered_set_test ${Unordered_set_Test_Src})

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")