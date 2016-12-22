cmake_minimum_required(VERSION 3.6)
project(superbubble_performance)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/")

include_directories(include)
include_directories(external)

set(SOURCE_FILES
        src/main.cpp
        src/io/FastaParser.cpp
        src/io/FastaParser.h
        src/io/DotExport.h
        src/graph/GraphConstructor.cpp
        src/graph/GraphConstructor.h
        src/algorithm/GraphCompressor.cpp
        src/algorithm/GraphCompressor.h
        src/io/Database.cpp
        src/io/Database.h
        src/graph/GraphIndexer.cpp
        src/graph/GraphIndexer.h
        src/PipelineRunner.cpp
        src/PipelineRunner.h
        src/cli/CliOptions.cpp
        src/cli/CliOptions.h
        src/cli/OptionContainer.h
        src/algorithm/Tarjan.h
        src/algorithm/Tarjan.cpp
        src/algorithm/superbubble/SuperBubble.cpp
        src/algorithm/superbubble/SuperBubble.h
        src/algorithm/superbubble/SB_Linear.cpp
        src/algorithm/superbubble/SB_Linear.h)

set(SQLITE3_FILES
        external/sqlite3/sqlite3.h
        external/sqlite3/sqlite3.c)


add_library(sqlite3 ${SQLITE3_FILES})

add_executable(sbp ${SOURCE_FILES})

SET_TARGET_PROPERTIES(sbp PROPERTIES LINKER_LANGUAGE CXX)
SET_TARGET_PROPERTIES(sqlite3 PROPERTIES LINKER_LANGUAGE CXX)

TARGET_LINK_LIBRARIES(sqlite3 dl)
TARGET_LINK_LIBRARIES(sbp sqlite3)

####################################
# GTest framework for unit testing #
####################################
enable_testing()
find_package(GTest QUIET)

if( GTEST_FOUND )
    include_directories(${GTEST_INCLUDE_DIRS})

    set(TEST_FILES
            tests/main.cpp tests/main.cpp)

    add_executable(
            sbp_tests
            ${TEST_FILES})

    target_link_libraries(sbp_tests gtest)
    add_test(sbp_tests ${TEST_FILES})
else()
    message(WARNING, "Google Test package not found. Unit tests will not be compiled...")
endif()