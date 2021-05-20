cmake_minimum_required (VERSION 3.5)
project (HYPERCUBES_TEST_Partitioning1D)

set(CMAKE_CXX_STANDARDS 14)

add_executable(test_partitioning1D test_partitioning1D.cpp)
target_link_libraries(test_partitioning1D q1D boost_test_helper)

add_executable(test_q1D test_q1D.cpp)
target_link_libraries(test_q1D q1D boost_test_helper)

add_executable(test_hbb1D test_hbb1D.cpp)
target_link_libraries(test_hbb1D hbb1D boost_test_helper)

add_executable(test_plain1D test_plain1D.cpp)
target_link_libraries(test_plain1D plain1D
                                   boost_test_helper)


enable_testing()
add_test(partitioning1D test_partitioning1D -r confirm)
add_test(q1D test_q1D -r confirm)
add_test(hbb1D test_hbb1D -r confirm)
add_test(plain1D test_plain1D -r confirm)
