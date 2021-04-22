cmake_minimum_required (VERSION 3.5)
project(HYPERCUBES)
set(CMAKE_CXX_STANDARDS 17)

include_directories(./include )
add_library(eo src/eo.cpp)
add_library(partitioning1D src/1D/partitioning1D.cpp)
add_library(q1D src/1D/q1D.cpp)
add_library(geometry src/geometry.cpp)



add_subdirectory(tests)
