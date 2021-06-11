cmake_minimum_required (VERSION 3.5)
project (HYPERCUBES_TEST_API)

add_library(fixtures1D fixtures1D.cpp)
add_library(fixtures2D fixtures2D.cpp)
add_library(fixtures4D fixtures4D.cpp)
target_link_libraries(fixtures1D partition_predicates)
target_link_libraries(fixtures2D partition_predicates)
target_link_libraries(fixtures4D partition_predicates)


add_library(facade INTERFACE)
target_link_libraries(facade INTERFACE memory_layout
                                       partition_tree_allocations
                                       level_swap
                                       boost_test_helper
                                       partition_tree)

add_library(allD_fixtures INTERFACE)
target_link_libraries(allD_fixtures INTERFACE fixtures1D
                                              fixtures2D
                                              fixtures4D)
                                     
add_executable(test_api_partition_tree test_partition_tree.cpp)
target_link_libraries(test_api_partition_tree facade allD_fixtures)

add_executable(test_api_nchildren_tree test_nchildren_tree.cpp)
target_link_libraries(test_api_nchildren_tree facade allD_fixtures)

add_executable(test_api_size_tree test_size_tree.cpp)
target_link_libraries(test_api_size_tree facade allD_fixtures)

add_executable(test_api_offset_tree test_offset_tree.cpp)
target_link_libraries(test_api_offset_tree facade allD_fixtures)

set(CMAKE_CXX_STANDARDS 14)

enable_testing()
add_test(api_partition_tree test_api_partition_tree -r confirm)
add_test(api_nchildren_tree test_api_nchildren_tree -r confirm)
add_test(api_size_tree test_api_size_tree -r confirm)
add_test(api_offset_tree test_api_offset_tree -r confirm)
