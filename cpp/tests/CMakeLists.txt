cmake_minimum_required (VERSION 3.5)
project (HYPERCUBES_TEST)

set(CMAKE_CXX_STANDARDS 17)

include_directories(../include .)
add_library(utils utils.cpp)

add_executable(test_eo test_eo.cpp)
target_link_libraries(test_eo eo utils geometry)

add_executable(test_rilist test_rilist.cpp)
target_link_libraries(test_rilist utils)

add_executable(test_partitioning1D test_partitioning1D.cpp)
target_link_libraries(test_partitioning1D q1D)

add_executable(test_q1D test_q1D.cpp)
target_link_libraries(test_q1D q1D)
enable_testing()
add_test(eo test_eo -r confirm)
add_test(rilist test_rilist -r confirm)
add_test(partitioning1D test_partitioning1D -r confirm)
add_test(q1D test_q1D -r confirm)
