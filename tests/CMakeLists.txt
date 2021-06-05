cmake_minimum_required (VERSION 3.5)
project (HYPERCUBES_TEST)

set(CMAKE_CXX_STANDARDS 14)

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

add_executable(test_kvtree test_kvtree.cpp)
target_link_libraries(test_kvtree boost_test_helper -lgcov)

add_executable(test_partitioners test_partitioners.cpp)
target_link_libraries(test_partitioners partitioners
                                        boost_test_helper)

add_executable(test_partition_tree test_partition_tree.cpp)
target_link_libraries(test_partition_tree partition_tree
                                          boost_test_helper
                                          test_utils)

add_executable(test_pt_full test_pt_full.cpp)
target_link_libraries(test_pt_full partition_tree
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


add_executable(test_level_swap test_level_swap.cpp)
target_link_libraries(test_level_swap boost_test_helper
                                      level_swap
                                      fixtures)

add_executable(test_memoisation test_memoisation.cpp)
target_link_libraries(test_memoisation boost_test_helper
                                       partition_tree)

add_executable(test_geometry test_geometry.cpp)
target_link_libraries(test_geometry boost_test_helper
                                    geometry)                                     
                                     
# Adding compile options for coverage for some tests.
# This is needed because some code exists only as template.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(test_dimensionalise PRIVATE "-fprofile-arcs"
                                                     "-ftest-coverage")
  target_compile_options(test_tree PRIVATE "-fprofile-arcs"
                                           "-ftest-coverage")

  target_compile_options(test_kvtree PRIVATE "-fprofile-arcs"
                                             "-ftest-coverage")


  target_compile_options(test_size_tree_iterator PRIVATE "-fprofile-arcs"
                                                         "-ftest-coverage")

  target_compile_options(test_level_swap PRIVATE "-fprofile-arcs"
                                                  "-ftest-coverage")

endif()
###########
# TESTING #
###########

add_subdirectory(1D)
add_subdirectory(api)
enable_testing()
# Ordered by duration, asc
add_test(geometry test_geometry -r confirm)
add_test(memoisation test_memoisation)
add_test(comments test_comments -r confirm)
add_test(tree test_tree -r confirm)
add_test(ktree test_kvtree -r confirm)
add_test(partitioners test_partitioners -r confirm)
add_test(size_tree_iterator test_size_tree_iterator -r confirm)
add_test(bool_maybe test_bool_maybe -r confirm)
add_test(level_swap test_level_swap -r confirm)
add_test(partition_tree test_partition_tree -r confirm)
add_test(rilist test_rilist -r confirm)
add_test(eo test_eo -r confirm)
add_test(eo_partitioning test_eo_partitioning -r confirm)
add_test(dimensionalise test_dimensionalise -r confirm)
add_test(partition_tree_allocations test_partition_tree_allocations -r confirm)
add_test(partition_predicates test_partition_predicates -r confirm)
add_test(pt_full test_pt_full -r confirm)
