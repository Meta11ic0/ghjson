cmake_minimum_required(VERSION 3.10)
project(test)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(ghjson STATIC ghjson.cpp)

add_executable(test test.cpp)
target_link_libraries(test ghjson)
