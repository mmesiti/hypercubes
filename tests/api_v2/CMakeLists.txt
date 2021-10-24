cmake_minimum_required (VERSION 3.5)
project (HYPERCUBES_TEST_API)

add_library(facade_v2 INTERFACE)
target_link_libraries(facade_v2 INTERFACE tree_transform
                                          transformer
                                          transform_requests
                                          boost_test_helper)

add_executable(test_tree_transform test_tree_transform.cpp)
target_link_libraries(test_tree_transform facade_v2)

add_executable(test_transformer test_transformer.cpp)
target_link_libraries(test_transformer facade_v2)

add_executable(test_transform_requests test_transform_requests.cpp)
target_link_libraries(test_transform_requests facade_v2)

set(CMAKE_CXX_STANDARDS 14)

enable_testing()
add_test(tree_transform test_tree_transform -r confirm)
add_test(transformer test_transformer -r confirm)
add_test(transform_requests test_transform_requests -r confirm)
