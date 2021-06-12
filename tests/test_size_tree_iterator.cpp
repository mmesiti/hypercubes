#include "fixtures.hpp"
#include "test_utils.hpp"
#include "trees/kvtree.hpp"
#include "trees/partition_tree.hpp"
#include "trees/partition_tree_allocations.hpp"
#include "trees/size_tree_iterator.hpp"
#include "trees/tree.hpp"
#include "utils/utils.hpp"
#include <boost/test/unit_test.hpp>

using namespace hypercubes::slow;
using namespace hypercubes::slow::internals;

struct SimpleSizeTree {

  std::pair<int, int> mp(int a, int b) { return std::make_pair(a, b); }
  TreeP<std::pair<int, int>> tree;
  SimpleSizeTree()
      : tree(mt(mp(0, 32), {mt(mp(0, 16), {mt(mp(0, 8), {}),   //
                                           mt(mp(1, 8), {})}), //
                            mt(mp(1, 16), {mt(mp(0, 8), {}),   //
                                           mt(mp(1, 8), {})})})){};
};

BOOST_AUTO_TEST_SUITE(test_size_tree_iterator)

BOOST_FIXTURE_TEST_CASE(test_iterate_simple1, SimpleSizeTree) {

  Indices idx{0, 0, 0}, idxp{0, 0, 1};

  BOOST_TEST(next(tree, idx) == idxp);
}
BOOST_FIXTURE_TEST_CASE(test_iterate_simple2, SimpleSizeTree) {

  Indices idx{0, 0, 7}, idxp{0, 1, 0};

  BOOST_TEST(next(tree, idx) == idxp);
}
BOOST_FIXTURE_TEST_CASE(test_iterate_simple3, SimpleSizeTree) {

  Indices idx{0, 1, 7}, idxp{1, 0, 0};

  BOOST_TEST(next(tree, idx) == idxp);
}

BOOST_FIXTURE_TEST_CASE(test_iterate_size_tree_with_holes, Part1D42) {

  int isite = 0;

  auto size_tree =
      get_size_tree(prune_tree(get_nchildren_alloc_tree(t), //
                               [&](Indices idx) -> BoolM {
                                 return no_bulk_borders(partitioners, idx);
                               }));
  auto no_sites = truncate_tree(size_tree,                     //
                                get_max_depth(size_tree) - 1); //
  while (isite + 1 < haloonly1D.size()) {
    int i, ip;
    Indices idx, idxp;
    std::tie(i, idx) = haloonly1D[isite];
    std::tie(ip, idxp) = haloonly1D[isite + 1];

    BOOST_TEST(no_sites->n.second == 16);
    BOOST_TEST(next(no_sites, idx) == idxp);
    ++isite;
  };
}

BOOST_FIXTURE_TEST_CASE(test_offset_iterator_match, Part1D42) {
  auto size_tree =
      get_size_tree(prune_tree(get_nchildren_alloc_tree(t), //
                               [&](Indices idx) -> BoolM {
                                 return no_bulk_borders(partitioners, idx);
                               }));
  auto no_sites = truncate_tree(size_tree,                     //
                                get_max_depth(size_tree) - 1); //

  auto idx = get_start_idxs(no_sites);
  auto offset_tree = get_offset_tree(no_sites);
  int expected_offset = 0;
  while (idx != get_end_idxs()) {
    int offset = get_offset(offset_tree, idx);
    BOOST_TEST(offset == expected_offset);
    idx = next(no_sites, idx);
    expected_offset++;
  }
}

BOOST_FIXTURE_TEST_CASE(test_offset_indices_roundtrip, Part1D42) {
  auto size_tree =
      get_size_tree(prune_tree(get_nchildren_alloc_tree(t), //
                               [&](Indices idx) -> BoolM {
                                 return no_bulk_borders(partitioners, idx);
                               }));
  auto no_sites = truncate_tree(size_tree,                     //
                                get_max_depth(size_tree) - 1); //

  Indices idx = get_start_idxs(no_sites);
  auto offset_tree = get_offset_tree(no_sites);
  int expected_offset = 0;
  while (idx != get_end_idxs()) {
    int offset = get_offset(offset_tree, idx);
    Indices indices_roundtrip = get_indices(offset_tree, offset);
    BOOST_TEST(idx == indices_roundtrip);
    idx = next(no_sites, idx);
  }
}
BOOST_AUTO_TEST_SUITE_END()
