#include "api/memory_layout.hpp"
#include "geometry/geometry.hpp"
#include <boost/test/unit_test.hpp>

using namespace hypercubes::slow;
namespace pm = hypercubes::slow::partitioner_makers;

BOOST_AUTO_TEST_SUITE(test_ragged_permutation)

BOOST_AUTO_TEST_CASE(test_ragged_1Dskeleton) {

  enum { X };

  /* Skeleton tree (excluding leaves):
  **       (root)
  **      /   |  \
  **     0    1   2 "Vec X"
  **    / \  / \  |
  **    0 1  0 1  0 "X"
  */
  auto skeleton_tree = PartitionTree({5},
                                     {pm::QOpen("Vec X", X, 3), //
                                      pm::Plain("X", X),        //
                                      pm::Site()},              //
                                     {})
                           .skeleton_tree();

  /* Permuted Skeleton tree (excluding leaves):
  **       (root)
  **      /      \
  **      0      1   "X"
  **    / | \   / \
  **    0 1 2   0  1 "Vec X"
  */
  auto permuted_skeleton_tree = skeleton_tree.permute({"X", "Vec X", "Site"});

  BOOST_TEST(skeleton_tree.get_nchildren({0}) == 2);
  BOOST_TEST(skeleton_tree.get_nchildren({1}) == 2);
  BOOST_TEST(skeleton_tree.get_nchildren({2}) == 1);

  BOOST_TEST(permuted_skeleton_tree.get_nchildren({0}) == 3);
  BOOST_TEST(permuted_skeleton_tree.get_nchildren({1}) == 2);
}

BOOST_AUTO_TEST_SUITE_END()
