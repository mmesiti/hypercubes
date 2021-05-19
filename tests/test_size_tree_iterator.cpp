#include "fixtures.hpp"
#include "partition_tree.hpp"
#include "partition_tree_allocations.hpp"
#include "size_tree_iterator.hpp"
#include "test_utils.hpp"
#include "tree.hpp"
#include "utils.hpp"
#include <boost/test/unit_test.hpp>

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
  auto haloonly1D = [&]() {
    vector<std::pair<int, Indices>> res;

    auto reorder_sites = [](vector<int> _border_sites) {
      int i = 1, step = 0;
      vector<int> out;
      while (i < _border_sites.size()) {
        out.push_back(_border_sites[i]);
        i += (step % 2 == 0) ? 3 : -1;
        ++step;
      }
      return out;
    };

    auto get_halo_idx = [&](int _bs) {
      auto itwg = get_indices_tree_with_ghosts(t, Coordinates{_bs});
      return get_relevant_indices_flat(itwg);
    };

    for (auto bs : reorder_sites(border_sites)) {
      for (auto halodim_idx : get_halo_idx(bs)) {
        int halodim;
        Indices idx;
        std::tie(halodim, idx) = halodim_idx;
        if (halodim)
          res.push_back(std::make_pair(bs, idx));
      }
    }
    return res;
  }();

  int isite = 0;

  auto size_tree = get_size_tree(
      get_nchildren_alloc_tree(t, //
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

    BOOST_TEST(no_sites->n.first == 16);
    BOOST_TEST(next(no_sites, idx) == idxp);
    ++isite;
  };
}

BOOST_AUTO_TEST_SUITE_END()
