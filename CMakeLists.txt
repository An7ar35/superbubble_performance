cmake_minimum_required(VERSION 3.6)
project(superbubble_performance)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(include)

set(SOURCE_FILES
        src/main.cpp
        src/io/FastaParser.cpp
        src/io/FastaParser.h src/io/GraphParser.cpp src/io/GraphParser.h src/io/DotExport.cpp src/io/DotExport.h src/io/GraphExport.cpp src/io/GraphExport.h src/graph/GraphConstructor.cpp src/graph/GraphConstructor.h src/algorithm/GraphCompressor.cpp src/algorithm/GraphCompressor.h src/algorithm/EulerWalk.cpp src/algorithm/EulerWalk.h src/algorithm/Comparator.cpp src/algorithm/Comparator.h)

add_executable(superbubble_performance ${SOURCE_FILES})