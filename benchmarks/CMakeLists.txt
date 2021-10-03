cmake_minimum_required (VERSION 3.5)
project (HYPERCUBES_TEST_API)
set(CMAKE_CXX_STANDARDS 14)

add_executable(grid_like grid_like.cpp)
target_link_libraries(grid_like memory_layout
                                partition_tree_allocations
                                partition_predicates
                                level_swap
                                partition_tree)
