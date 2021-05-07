cmake_minimum_required (VERSION 3.5)
project(HYPERCUBES)
set(CMAKE_CXX_STANDARDS 14)

SET(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
SET(GCC_COVERAGE_LINK_FLAGS    "-lgcov")

# dirty
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

include_directories(./include )
add_library(eo src/eo.cpp)

add_library(geometry src/geometry.cpp)
add_library(print_utils src/print_utils.cpp)
add_library(partitioning src/partitioning.cpp)


add_library(partitioners src/partitioners.cpp)
target_link_libraries(partitioners eo_partitioning
  q1D
  hbb1D
  plain1D)

add_library(partitioning1D src/1D/partitioning1D.cpp)
target_link_libraries(partitioning1D geometry)

add_library(q1D src/1D/q1D.cpp)
target_link_libraries(q1D partitioning1D partitioning)

add_library(hbb1D src/1D/hbb1D.cpp)
target_link_libraries(hbb1D partitioning1D partitioning)

add_library(plain1D src/1D/plain1D.cpp)
target_link_libraries(plain1D partitioning1D partitioning)

add_library(eo_partitioning src/eo_partitioning.cpp)
target_link_libraries(eo_partitioning geometry eo print_utils partitioning)

add_library(partition_class_tree src/partition_class_tree.cpp)
target_link_libraries(partition_class_tree partitioners)

add_library(partition_predicates src/partition_predicates.cpp)
target_link_libraries(partition_predicates partitioners)

add_library(partition_tree src/partition_tree.cpp)
target_link_libraries(partition_tree partition_predicates partitioners)

add_library(size_tree_iterator src/size_tree_iterator.cpp)
target_link_libraries(size_tree_iterator geometry)

add_custom_target(coverage
     COMMAND lcov --capture
             --directory CMakeFiles
             --directory tests/CMakeFiles/test_dimensionalise.dir
             --directory tests/CMakeFiles/test_tree.dir
             --output-file coverage.info
             --exclude '/usr/include/\*' # Hack to remove system headers
     COMMAND genhtml --demangle-cpp coverage.info
     BYPRODUCTS home
                index-sort-f.html
                index-sort-l.html
                index.html
                gcov.css
                coverage.info)

add_subdirectory(tests)
