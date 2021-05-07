#include "fixtures.hpp"
#include "partition_class_tree.hpp"
#include "partition_tree.hpp"
#include "size_tree_iterator.hpp"
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

BOOST_FIXTURE_TEST_CASE(test_iterate_simple, SimpleSizeTree) {

  Indices idx{0, 0, 0}, idxp{0, 0, 1};

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
        i += (step % 2 == 0) ? -1 : 3;
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

  int i = 0;
  while (i + 1 < haloonly1D.size()) {
    int i, ip;
    Indices idx, idxp;
    std::tie(i, idx) = haloonly1D[i];
    std::tie(ip, idxp) = haloonly1D[i + 1];

    auto size_tree = get_size_tree(t, [&](Indices idx) -> BoolM {
      return no_bulk_borders(partitioners, idx);
    });
    BOOST_TEST(size_tree->n.first == 15);
    BOOST_TEST(next(size_tree, idx) == idxp);
  };
}

BOOST_AUTO_TEST_SUITE_END()
