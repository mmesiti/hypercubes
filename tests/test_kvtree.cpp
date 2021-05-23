
#include "kvtree.hpp"
#include <boost/test/unit_test.hpp>

using namespace hypercubes::slow;
BOOST_AUTO_TEST_SUITE(test_tree_kv)

BOOST_AUTO_TEST_CASE(test_collapse_level_kv) {

  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };
  auto t = mt(mp(0, 1), {mt(mp(3, 2), {mt(mp(5, 3), {mt(mp(1, 4), {}),     //
                                                     mt(mp(2, 5), {})}),   //
                                       mt(mp(6, 6), {mt(mp(3, 7), {}),     //
                                                     mt(mp(4, 8), {})})}), //
                         mt(mp(4, 9), {mt(mp(7, 10), {mt(mp(5, 11), {}),   //
                                                      mt(mp(7, 12), {})}), //
                                       mt(mp(8, 13), {mt(mp(8, 14), {}),   //
                                                      mt(mp(9, 15), {})})})});

  auto tcollapsed0_exp = mt(mp(0, 1), {mt(mp(3, 3), {mt(mp(1, 4), {}),   //
                                                     mt(mp(2, 5), {})}), //
                                       mt(mp(4, 10), {mt(mp(5, 11), {}), //
                                                      mt(mp(7, 12), {})})});

  auto tcollapsed0 = collapse_level(t,                       //
                                    1 /*level to collapse*/, //
                                    0 /*child to replace*/);

  BOOST_TEST(*tcollapsed0_exp == *tcollapsed0);
}

BOOST_AUTO_TEST_CASE(test_bring_level_on_top_kv) {
  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };
  auto t = mt(mp(0, 1), {mt(mp(20, 2), {mt(mp(22, 3), {mt(mp(24, 4), {}), //
                                                       mt(mp(25, 5), {})})}),
                         mt(mp(21, 9), {mt(mp(23, 3), {mt(mp(24, 11), {}), //
                                                       mt(mp(25, 12), {})})})});

  auto transf_expected =
      mt(mp(0, 3), {mt(mp(24, 1), {mt(mp(20, 2), {mt(mp(22, 4), {})}), //
                                   mt(mp(21, 9), {mt(mp(23, 11), {})})}),
                    mt(mp(25, 1), {mt(mp(20, 2), {mt(mp(22, 5), {})}), //
                                   mt(mp(21, 9), {mt(mp(23, 12), {})})})});

  auto transf = bring_level_on_top(t, 2);

  BOOST_TEST(*transf_expected == *transf);
}

BOOST_AUTO_TEST_CASE(test_swap_levels_kv) {

  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };
  auto t = mt(mp(0, 1), {mt(mp(10, 2), {mt(mp(12, 3), {mt(mp(14, 5), {}),     //
                                                       mt(mp(15, 6), {})})}), //
                         mt(mp(11, 4), {mt(mp(13, 3), {mt(mp(14, 7), {}),     //
                                                       mt(mp(15, 8), {})})})});
  vector<int> new_level_ordering{2, 0, 1};

  auto newt_exp =
      mt(mp(0, 3), {mt(mp(14, 1), {mt(mp(10, 2), {mt(mp(12, 5), {})}),   //
                                   mt(mp(11, 4), {mt(mp(13, 7), {})})}), //
                    mt(mp(15, 1), {mt(mp(10, 2), {mt(mp(12, 6), {})}),   //
                                   mt(mp(11, 4), {mt(mp(13, 8), {})})})});

  auto newt = swap_levels(t, new_level_ordering);

  BOOST_TEST(*newt == *newt_exp);
}

BOOST_AUTO_TEST_SUITE_END()
