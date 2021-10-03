cmake_minimum_required (VERSION 3.5)
project (HYPERCUBES_TEST_API)

add_library(facade_v2 INTERFACE)
target_link_libraries(facade_v2 INTERFACE tree_transform
                                          boost_test_helper)

add_executable(test_tree_transform test_tree_transform.cpp)
target_link_libraries(test_tree_transform facade_v2)

set(CMAKE_CXX_STANDARDS 14)

enable_testing()
add_test(tree_transform test_tree_transform -r confirm)
