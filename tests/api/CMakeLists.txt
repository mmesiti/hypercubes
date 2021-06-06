cmake_minimum_required (VERSION 3.5)
project (HYPERCUBES_TEST_API)

add_executable(test_grid_like test_grid_like.cpp)
target_link_libraries(test_grid_like memory_layout boost_test_helper partition_tree)

set(CMAKE_CXX_STANDARDS 14)

enable_testing()
add_test(grid_like test_grid_like -r confirm)
