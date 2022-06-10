cmake_minimum_required (VERSION 3.5)
project (HYPERCUBES_TEST_API)


add_executable(test_tree_transform test_tree_transform.cpp)
target_link_libraries(test_tree_transform tree_transform
                                          boost_test_helper
                                          -lgcov)


add_executable(test_transformer test_transformer.cpp)
target_link_libraries(test_transformer transformer
                                       boost_test_helper)

add_executable(test_transform_requests test_transform_requests.cpp)
target_link_libraries(test_transform_requests transform_requests
                                              transform_network
                                              boost_test_helper)

add_executable(test_transform_request_makers test_transform_request_makers.cpp)
target_link_libraries(test_transform_request_makers transform_request_makers
                                                    boost_test_helper)

add_executable(test_transform_network test_transform_network.cpp)
target_link_libraries(test_transform_network transform_network
                                             transform_requests
                                             transform_request_makers
                                             boost_test_helper)

# Adding compile options for coverage for some tests.
# This is needed because some code exists only as template.
# (Note: this list might need to be lengthened.)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(test_tree_transform PRIVATE "-fprofile-arcs"
                                                     "-ftest-coverage")

endif()

set(CMAKE_CXX_STANDARDS 14)

enable_testing()
add_test(tree_transform test_tree_transform -r confirm)
add_test(transformer test_transformer -r confirm)
add_test(transform_requests test_transform_requests -r confirm)
add_test(transform_request_makers test_transform_request_makers -r confirm)
add_test(transform_network test_transform_network -r confirm)
