cmake_minimum_required (VERSION 3.5)
project(HYPERCUBES)
set(CMAKE_CXX_STANDARDS 14)

include_directories(./include )
add_library(eo src/eo.cpp)

add_library(geometry src/geometry.cpp)

add_library(partitioning1D src/1D/partitioning1D.cpp)
target_link_libraries(partitioning1D geometry)

add_library(q1D src/1D/q1D.cpp)
target_link_libraries(q1D partitioning1D)

add_library(hbb1D src/1D/hbb1D.cpp)
target_link_libraries(hbb1D partitioning1D)

add_library(plain1D src/1D/plain1D.cpp)
target_link_libraries(plain1D partitioning1D)

add_library(eo_partitioning src/eo_partitioning.cpp)
target_link_libraries(eo_partitioning geometry eo)

add_subdirectory(tests)
