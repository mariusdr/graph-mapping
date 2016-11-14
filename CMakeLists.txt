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

message(STATUS "COMPILE COMMAND: ${CMAKE_CXX_FLAGS}")

add_executable(demo src/main.cpp)
target_link_libraries(demo NetworKit-Core-Opt)

