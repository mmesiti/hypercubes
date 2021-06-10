#include "selectors/bool_maybe.hpp"
#include "trees/kvtree.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <stdexcept>

using namespace hypercubes::slow::internals;
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

BOOST_AUTO_TEST_CASE(test_number_children) {
  auto tnoidx = mt(1, {mt(2, {mt(3, {mt(5, {}),     //
                                     mt(6, {})})}), //
                       mt(4, {mt(3, {mt(7, {}),     //
                                     mt(8, {})})})});

  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };
  auto expt = mt(mp(0, 1), {mt(mp(0, 2), {mt(mp(0, 3), {mt(mp(0, 5), {}),     //
                                                        mt(mp(1, 6), {})})}), //
                            mt(mp(1, 4), {mt(mp(0, 3), {mt(mp(0, 7), {}),     //
                                                        mt(mp(1, 8), {})})})});

  auto t = number_children(tnoidx);
  BOOST_TEST(*expt == *t);
}

BOOST_AUTO_TEST_CASE(test_prune_tree) {

  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };
  auto tfull =
      mt(mp(0, 1), {mt(mp(0, 2), {mt(mp(0, 3), {mt(mp(0, 5), {}),     //
                                                mt(mp(1, 6), {})})}), //
                    mt(mp(1, 4), {mt(mp(10, 3), {mt(mp(0, 7), {}),    //
                                                 mt(mp(1, 8), {})})})});

  auto predicate = [](hypercubes::slow::Indices idxs) {
    using hypercubes::slow::BoolM;
    if (idxs.size() > 0) {
      if (idxs[0] != 1)
        return BoolM::F;
      else
        return BoolM::T;
    }

    return BoolM::M;
  };
  auto expt = mt(mp(0, 1), {mt(mp(1, 4), {mt(mp(10, 3), {mt(mp(0, 7), {}), //
                                                         mt(mp(1, 8), {})})})});

  auto t = prune_tree(tfull, predicate);
  BOOST_TEST(*t == *expt);
}

BOOST_AUTO_TEST_CASE(test_select_subtree_kv) {
  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };
  auto tfull =
      mt(mp(0, 1), {mt(mp(0, 2), {mt(mp(0, 3), {mt(mp(0, 5), {}),     //
                                                mt(mp(1, 6), {})})}), //
                    mt(mp(1, 4), {mt(mp(10, 3), {mt(mp(0, 7), {}),    //
                                                 mt(mp(1, 8), {})})})});
  auto exp_selected = mt(mp(10, 3), {mt(mp(0, 7), {}), //
                                     mt(mp(1, 8), {})});

  auto selected = select_subtree_kv(tfull, vector<int>{1, 10});

  BOOST_TEST(*selected == *exp_selected);
}

BOOST_AUTO_TEST_CASE(test_select_subtree_kv_throws) {
  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };
  auto tree = mt(mp(10, 3), {mt(mp(0, 7), {}), //
                             mt(mp(10, 8), {})});

  BOOST_CHECK_THROW(select_subtree_kv(tree, vector<int>{1}), //
                    std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_shift_tree) {
  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };
  auto tree = mt(mp(0, 1), {mt(mp(0, 2), {mt(mp(0, 3), {mt(mp(0, 5), {}),     //
                                                        mt(mp(1, 6), {})})}), //
                            mt(mp(1, 4), {mt(mp(10, 3), {mt(mp(0, 7), {}),    //
                                                         mt(mp(1, 8), {})})})});
  auto tshift_exp =
      mt(mp(0, 11), {mt(mp(0, 12), {mt(mp(0, 13), {mt(mp(0, 15), {}),     //
                                                   mt(mp(1, 16), {})})}), //
                     mt(mp(1, 14), {mt(mp(10, 13), {mt(mp(0, 17), {}),    //
                                                    mt(mp(1, 18), {})})})});
  auto tshift = shift_tree(tree, 10);
  BOOST_TEST(*tshift == *tshift_exp);
}

BOOST_AUTO_TEST_SUITE_END()
