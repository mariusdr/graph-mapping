cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(demo CXX)
message(STATUS "PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -O3 -fopenmp")

################################################################################
# Find NetworKit                                                               #
################################################################################
set (NETWORKIT_INCLUDE_DIR ~/Coding/Uni/BAProject/NetworKit/include/NetworKit)
set (NETWORKIT_LIBRARY_DIR ~/Coding/Uni/BAProject/NetworKit)

message(STATUS "NETWORKIT_INCLUDE_DIR: ${NETWORKIT_INCLUDE_DIR}")
message(STATUS "NETWORKIT_LIBRARY_DIR: ${NETWORKIT_LIBRARY_DIR}")

link_directories(${NETWORKIT_LIBRARY_DIR})
include_directories(${NETWORKIT_INCLUDE_DIR})

################################################################################
# Find GoogleTest                                                              #
################################################################################
enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

################################################################################
# Find Boost Program_options                                                   #
################################################################################
find_package(Boost REQUIRED program_options)
include_directories(${Boost_INCLUDE_DIRS})

################################################################################

message(STATUS "COMPILE COMMAND: ${CMAKE_CXX_FLAGS}")

set (SOURCE_FILES
  src/main.cpp
  src/CommTime.cpp
  src/GreedyMapping.cpp
  src/Benchmark.cpp
  src/InitialMapping.cpp
  src/RandomMapping.cpp
  src/IOUtil.cpp
  src/Demo.cpp
)

set (TEST_FILES
  src/CommTime.cpp
  src/GreedyMapping.cpp
  src/test/test.cpp
  src/test/CommTimeTest.cpp
  src/test/GreedyMappingTest.cpp
)

add_executable(computeMapping ${SOURCE_FILES})
target_link_libraries(computeMapping NetworKit-Core-Opt ${Boost_PROGRAM_OPTIONS_LIBRARY})

add_executable(testRunner ${TEST_FILES})
target_link_libraries(testRunner ${GTEST_BOTH_LIBRARIES})
target_link_libraries(testRunner NetworKit-Core-Opt)
add_test(testRunner testRunner)
