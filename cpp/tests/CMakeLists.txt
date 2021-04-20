cmake_minimum_required (VERSION 3.5)
project (HYPERCUBES_TEST)

set(CMAKE_CXX_STANDARDS 17)

include_directories(../include .)
add_executable(test_eo test_eo.cpp)
add_executable(test_rilist test_rilist.cpp)


enable_testing()
add_test(eo test_eo -r detailed)
add_test(rilist test_rilist -r detailed)
