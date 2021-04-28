cmake_minimum_required (VERSION 3.5)
project (HYPERCUBES_TEST)

set(CMAKE_CXX_STANDARDS 14)

include_directories(../include .)
add_library(utils utils.cpp)
# To speed up compilation times,
# we compile boost/test only once.
add_library(boost_test_helper boost_test_helper.cpp)

add_executable(test_eo test_eo.cpp)
target_link_libraries(test_eo eo utils geometry boost_test_helper)

add_executable(test_rilist test_rilist.cpp)
target_link_libraries(test_rilist utils boost_test_helper)

add_executable(test_partitioning1D test_partitioning1D.cpp)
target_link_libraries(test_partitioning1D q1D boost_test_helper)

add_executable(test_q1D test_q1D.cpp)
target_link_libraries(test_q1D q1D boost_test_helper)

add_executable(test_hbb1D test_hbb1D.cpp)
target_link_libraries(test_hbb1D hbb1D boost_test_helper)

add_executable(test_plain1D test_plain1D.cpp)
target_link_libraries(test_plain1D plain1D boost_test_helper)

add_executable(test_eo_partitioning test_eo_partitioning.cpp)
target_link_libraries(test_eo_partitioning boost_test_helper eo_partitioning)

add_executable(test_dimensionalise test_dimensionalise.cpp)
target_link_libraries(test_dimensionalise q1D hbb1D plain1D boost_test_helper)

add_executable(test_comments test_comments.cpp)
target_link_libraries(test_comments q1D hbb1D plain1D eo_partitioning boost_test_helper)

add_executable(test_tree test_tree.cpp)
target_link_libraries(test_tree boost_test_helper)

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
