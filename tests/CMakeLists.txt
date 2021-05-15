cmake_minimum_required (VERSION 3.5)
project (HYPERCUBES_TEST)

set(CMAKE_CXX_STANDARDS 14)

include_directories(../include .)
add_library(test_utils test_utils.cpp)
add_library(fixtures fixtures.cpp)
target_link_libraries(fixtures partitioners partition_tree)
# To speed up compilation times,
# we compile boost/test only once.
add_library(boost_test_helper boost_test_helper.cpp)

add_executable(test_eo test_eo.cpp)
target_link_libraries(test_eo eo test_utils geometry boost_test_helper)

add_executable(test_rilist test_rilist.cpp)
target_link_libraries(test_rilist test_utils boost_test_helper)

add_executable(test_partitioning1D test_partitioning1D.cpp)
target_link_libraries(test_partitioning1D q1D boost_test_helper)

add_executable(test_q1D test_q1D.cpp)
target_link_libraries(test_q1D q1D boost_test_helper)

add_executable(test_hbb1D test_hbb1D.cpp)
target_link_libraries(test_hbb1D hbb1D boost_test_helper)

add_executable(test_plain1D test_plain1D.cpp)
target_link_libraries(test_plain1D plain1D
                                   boost_test_helper)

add_executable(test_eo_partitioning test_eo_partitioning.cpp)
target_link_libraries(test_eo_partitioning boost_test_helper
                                           eo_partitioning)

add_executable(test_dimensionalise test_dimensionalise.cpp)
target_link_libraries(test_dimensionalise q1D
                                          hbb1D
                                          plain1D
                                          boost_test_helper
                                          -lgcov)

add_executable(test_comments test_comments.cpp)
target_link_libraries(test_comments q1D hbb1D plain1D eo_partitioning boost_test_helper)

add_executable(test_tree test_tree.cpp)
target_link_libraries(test_tree boost_test_helper -lgcov)

add_executable(test_partitioners test_partitioners.cpp)
target_link_libraries(test_partitioners partitioners
                                        boost_test_helper)

add_executable(test_partition_tree test_partition_tree.cpp)
target_link_libraries(test_partition_tree partition_tree
                                          boost_test_helper
                                          test_utils)

add_executable(test_pct_full test_pct_full.cpp)
target_link_libraries(test_pct_full partition_tree
                                    boost_test_helper
                                    test_utils
                                    fixtures)

add_executable(test_partition_predicates test_partition_predicates.cpp)
target_link_libraries(test_partition_predicates fixtures #
                                                test_utils #
                                                boost_test_helper # 
                                                partition_predicates)


add_executable(test_partition_tree_allocations test_partition_tree_allocations.cpp)
target_link_libraries(test_partition_tree_allocations partition_tree_allocations
                                                      boost_test_helper
                                                      partition_tree
                                                      fixtures
                                                      test_utils)

add_executable(test_size_tree_iterator test_size_tree_iterator.cpp)
target_link_libraries(test_size_tree_iterator size_tree_iterator
                                              boost_test_helper
                                              test_utils
                                              fixtures)


add_executable(test_bool_maybe test_bool_maybe.cpp)
target_link_libraries(test_bool_maybe bool_maybe
                                      boost_test_helper)                                        


# Adding compile options for coverage for some tests.
# This is needed because some code exists only as template.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(test_dimensionalise PRIVATE "-fprofile-arcs"
                                                     "-ftest-coverage")
  target_compile_options(test_tree PRIVATE "-fprofile-arcs"
                                           "-ftest-coverage")
  target_compile_options(test_size_tree_iterator PRIVATE "-fprofile-arcs"
                                                         "-ftest-coverage")

endif()
###########
# TESTING #
###########

enable_testing()
add_test(eo test_eo -r confirm)
add_test(rilist test_rilist -r confirm)
add_test(partitioning1D test_partitioning1D -r confirm)
add_test(q1D test_q1D -r confirm)
add_test(hbb1D test_hbb1D -r confirm)
add_test(plain1D test_plain1D -r confirm)
add_test(eo_partitioning test_eo_partitioning -r confirm)
add_test(dimensionalise test_dimensionalise -r confirm)
add_test(comments test_comments -r confirm)
add_test(tree test_tree -r confirm)
add_test(partitioners test_partitioners -r confirm)
add_test(partition_tree test_partition_tree -r confirm)
add_test(pct_full test_pct_full -r confirm)
add_test(partition_predicates test_partition_predicates -r confirm)
add_test(partition_tree_allocations test_partition_tree_allocations -r confirm)
add_test(size_tree_iterator test_size_tree_iterator -r confirm)
add_test(bool_maybe test_bool_maybe -r confirm)
