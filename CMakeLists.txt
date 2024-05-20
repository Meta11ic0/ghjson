cmake_minimum_required(VERSION 3.10)
project(test)
set(CMAKE_CXX_STANDARD 11)

add_library(ghjson ghjson.cpp)
add_executable(test test.cpp)
target_link_libraries(test ghjson)
