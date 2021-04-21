cmake_minimum_required (VERSION 3.5)
project(HYPERCUBES)
set(CMAKE_CXX_STANDARDS 17)


include_directories(./include )
add_library(eo src/eo.cpp)
add_subdirectory(tests)
