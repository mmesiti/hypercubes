cmake_minimum_required (VERSION 3.5)
project(HYPERCUBES)
set(CMAKE_CXX_STANDARDS 14)



SET(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
SET(GCC_COVERAGE_LINK_FLAGS    "-lgcov")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
# dirty
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")
endif()

include_directories(./include )

# Base libraries
add_library(int_vec_wrappers src/geometry/int_vec_wrappers.cpp)
add_library(eo src/geometry/eo.cpp)
add_library(geometry src/geometry/geometry.cpp)
target_link_libraries(geometry int_vec_wrappers)
add_library(print_utils src/utils/print_utils.cpp)

# Partitioning
add_library(partitioning src/partitioners/partitioning.cpp)

## Multi-D partitioning
add_library(eo_partitioning src/partitioners/eo_partitioning.cpp)
target_link_libraries(eo_partitioning geometry
                                      eo
                                      print_utils
                                      partitioning)

add_library(site src/partitioners/site.cpp)
target_link_libraries(site geometry
                           partitioning)                                    

## 1-D partitioning
add_library(partitioning1D src/partitioners/1D/partitioning1D.cpp)
target_link_libraries(partitioning1D geometry)

add_library(q1D src/partitioners/1D/q1D.cpp)
target_link_libraries(q1D partitioning1D
                          partitioning)

add_library(hbb1D src/partitioners/1D/hbb1D.cpp)
target_link_libraries(hbb1D partitioning1D
                            partitioning)

add_library(plain1D src/partitioners/1D/plain1D.cpp)
target_link_libraries(plain1D partitioning1D
                              partitioning)

# Partitioners
add_library(partitioners src/partitioners/partitioners.cpp)
target_link_libraries(partitioners eo_partitioning
                                   q1D
                                   hbb1D
                                   plain1D
                                   site)

# Trees                                
add_library(partition_tree src/trees/partition_tree.cpp)
target_link_libraries(partition_tree partitioners)

add_library(bool_maybe src/selectors/bool_maybe.cpp)
add_library(intervals src/selectors/intervals.cpp)
target_link_libraries(intervals bool_maybe)
add_library(partition_predicates src/selectors/partition_predicates.cpp)
target_link_libraries(partition_predicates partitioners
                                           bool_maybe)

add_library(partition_tree_allocations src/trees/partition_tree_allocations.cpp)
target_link_libraries(partition_tree_allocations partition_predicates
                                                 int_vec_wrappers
                                                 partition_tree                                                  
                                                 partitioners)

add_library(size_tree_iterator src/trees/size_tree_iterator.cpp)
target_link_libraries(size_tree_iterator geometry partition_tree_allocations)

add_library(level_swap src/trees/level_swap.cpp)
target_link_libraries(level_swap partitioners)

# Tree transformation
add_library(tree_transform src/api_v2/tree_transform.cpp)

add_library(transform_network src/api_v2/transform_network.cpp)

add_library(transformer src/api_v2/transformer.cpp)
target_link_libraries(transformer tree_transform level_swap transform_network)

add_library(transform_requests src/api_v2/transform_requests.cpp)
target_link_libraries(transform_requests transformer)

add_library(transform_request_makers src/api_v2/transform_request_makers.cpp)
target_link_libraries(transform_request_makers transform_network
  transformer transform_requests)

add_library(selectors src/selectors/selectors.cpp)
target_link_libraries(selectors intervals)

# API
add_library(memory_layout src/api/memory_layout.cpp)
target_link_libraries(memory_layout geometry
                                    partitioners)


if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # TODO: add more directories when needed
add_custom_target(coverage
     COMMAND lcov --capture
             --directory CMakeFiles
             --directory tests/CMakeFiles/test_dimensionalise.dir
             --directory tests/CMakeFiles/test_tree.dir
             --directory tests/CMakeFiles/test_size_tree_iterator.dir
             --directory tests/CMakeFiles/test_level_swap.dir
             --directory tests/api_v2/CMakeFiles/test_tree_transform.dir
             --directory tests/api_v2/CMakeFiles/test_transformer.dir
             --directory tests/api_v2/CMakeFiles/test_transform_requests.dir
             --directory tests/api_v2/CMakeFiles/test_transform_network.dir
             --directory tests/api_v2/CMakeFiles/test_transform_request_makers.dir
             --directory tests/CMakeFiles/test_intervals.dir
             --directory tests/CMakeFiles/test_selectors.dir
             --output-file coverage.info
             --exclude '/usr/include/\*' # Hack to remove system headers
     COMMAND genhtml --demangle-cpp coverage.info
     BYPRODUCTS home
                index-sort-f.html
                index-sort-l.html
                index.html
                gcov.css
                coverage.info)
endif()
              
add_subdirectory(tests)
add_subdirectory(benchmarks)
