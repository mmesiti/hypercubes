#include "size_tree_iterator.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

using namespace hypercubes::slow;

struct SimpleSizeTree {

  std::pair<int, int> mp(int a, int b) { return std::make_pair(a, b); }
  TreeP<std::pair<int, int>> tree;
  SimpleSizeTree()
      : tree(mt(mp(32, 0), {mt(mp(16, 0), {mt(mp(8, 0), {}),   //
                                           mt(mp(8, 1), {})}), //
                            mt(mp(16, 1), {mt(mp(8, 0), {}),   //
                                           mt(mp(8, 1), {})})})){};
};

BOOST_AUTO_TEST_SUITE(test_size_tree_iterator)

BOOST_FIXTURE_TEST_CASE(test_iterate_simple, SimpleSizeTree) {

  Indices idx{0, 0, 0}, idxp{0, 0, 1};

  BOOST_TEST(next(tree, idx) == idxp);
}

// TODO:
// Add case "with holes", still missing in python version

BOOST_AUTO_TEST_SUITE_END()
