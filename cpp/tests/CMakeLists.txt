cmake_minimum_required (VERSION 3.5)
project (HYPERCUBES_TEST)

set(CMAKE_CXX_STANDARDS 17)

include_directories(../include)
add_executable(test_utils test_utils.cpp)


enable_testing()
add_test(utils test_utils -r detailed)
