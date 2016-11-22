cmake_minimum_required(VERSION 3.6)
project(superbubble_performance)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/main.cpp)

add_executable(superbubble_performance ${SOURCE_FILES})