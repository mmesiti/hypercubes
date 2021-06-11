cmake_minimum_required (VERSION 3.5)
project (HYPERCUBES_TEST_API)

add_library(fixtures1D fixtures1D.cpp)
add_library(fixtures2D fixtures2D.cpp)
add_library(fixtures4D fixtures4D.cpp)
target_link_libraries(fixtures1D partition_predicates)
target_link_libraries(fixtures2D partition_predicates)
target_link_libraries(fixtures4D partition_predicates)

add_executable(test_grid_like test_grid_like.cpp)
target_link_libraries(test_grid_like memory_layout
                                     partition_tree_allocations
                                     partition_predicates
                                     level_swap
                                     boost_test_helper
                                     fixtures1D
                                     fixtures2D
                                     fixtures4D
                                     partition_tree)
                                   
set(CMAKE_CXX_STANDARDS 14)

enable_testing()
add_test(grid_like test_grid_like -r confirm)
