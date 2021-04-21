cmake_minimum_required (VERSION 3.5)
project (HYPERCUBES_TEST)

set(CMAKE_CXX_STANDARDS 17)

include_directories(../include .)
add_library(utils utils.cpp)

add_executable(test_eo test_eo.cpp)
target_link_libraries(test_eo eo utils)

add_executable(test_rilist test_rilist.cpp)
target_link_libraries(test_rilist utils)

enable_testing()
add_test(eo test_eo -r detailed)
add_test(rilist test_rilist -r detailed)
