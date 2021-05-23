
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <sstream>

#include "tree.hpp"

using namespace hypercubes::slow;

BOOST_AUTO_TEST_SUITE(test_tree)

BOOST_AUTO_TEST_CASE(test_tree_equal) {

  auto t1 = mt(1, {mt(2, {}), //
                   mt(3, {mt(4, {})})});

  auto t2 = mt(1, {mt(2, {}), //
                   mt(3, {mt(4, {})})});

  BOOST_TEST(*t1 == *t2);
}

BOOST_AUTO_TEST_CASE(test_tree_notequal) {

  auto t1 = mt(1, {mt(2, {}), //
                   mt(3, {mt(4, {})})});

  auto t2 = mt(1, {mt(2, {}), //
                   mt(3, {})});

  BOOST_TEST(*t1 != *t2);
}

BOOST_AUTO_TEST_CASE(test_get_all_paths) {
  auto t = mt(1, {mt(2, {}),                     //
                  mt(3, {mt(4, {}), mt(5, {})}), //
                  mt(6, {mt(7, {mt(8, {})})})});
  auto res = get_all_paths(t);
  decltype(res) expected{{1, 2},    //
                         {1, 3, 4}, //
                         {1, 3, 5}, //
                         {1, 6, 7, 8}};

  BOOST_TEST(res == expected);
}

BOOST_AUTO_TEST_CASE(test_tree_str) {
  auto t = mt(1, {mt(2, {}),                     //
                  mt(3, {mt(4, {}), mt(5, {})}), //
                  mt(6, {mt(7, {mt(8, {})})})});

  auto res = tree_str(*t);
  std::string expected(
      "\n1"               //
      "\n  2\n"           // end of the '2' child
      "\n  3"             //
      "\n    4\n"         // end of the '4' child
      "\n    5\n\n"       // end of the '5' child, but also 3(45) subtree
      "\n  6"             //
      "\n    7"           //
      "\n      8\n\n\n"); // end of '8','7(8)' and '6(7(8))'
  BOOST_TEST(res == expected);
}

BOOST_AUTO_TEST_CASE(test_get_flat_list_of_subtrees) {

  auto t = mt(1, {
                     mt(2, {mt(3, {mt(4, {}),   //
                                   mt(5, {})}), //
                            mt(6, {mt(7, {}),   //
                                   mt(8, {})})}),
                     mt(9, {mt(10, {mt(11, {}),   //
                                    mt(12, {})}), //
                            mt(13, {mt(14, {}),   //
                                    mt(15, {})})}),
                 });

  auto res = get_flat_list_of_subtrees(t, 2);
  decltype(res) expected{mt(3, {mt(4, {}),   //
                                mt(5, {})}), //
                         mt(6, {mt(7, {}),   //
                                mt(8, {})}),
                         mt(10, {mt(11, {}),   //
                                 mt(12, {})}), //
                         mt(13, {mt(14, {}),   //
                                 mt(15, {})})};

  for (int i = 0; i < 4; ++i) {
    BOOST_TEST(*res[i] == *expected[i]);
  }
}

BOOST_AUTO_TEST_CASE(test_get_max_depth) {

  auto t = mt(1, {mt(2, {}),                     //
                  mt(3, {mt(4, {}), mt(5, {})}), //
                  mt(6, {mt(7, {mt(8, {})})})});

  int expected_max_depth = 4;

  BOOST_TEST(expected_max_depth == get_max_depth(t));
}

BOOST_AUTO_TEST_CASE(test_truncate_tree) {

  auto t = mt(1, {
                     mt(2, {mt(3, {mt(4, {}),   //
                                   mt(5, {})}), //
                            mt(6, {mt(7, {}),   //
                                   mt(8, {})})}),
                     mt(9, {mt(10, {mt(11, {}),   //
                                    mt(12, {})}), //
                            mt(13, {mt(14, {}),   //
                                    mt(15, {})})}),
                 });
  auto truncated_expected = mt(1, {
                                      mt(2, {mt(3, {}), //
                                             mt(6, {})}),
                                      mt(9, {mt(10, {}), //
                                             mt(13, {})}),
                                  });

  BOOST_TEST(*truncated_expected == *truncate_tree(t, 3));
}

BOOST_AUTO_TEST_CASE(test_get_leaves_list) {
  auto t = mt(1, {
                     mt(2, {mt(3, {mt(4, {}),   //
                                   mt(5, {})}), //
                            mt(6, {mt(7, {}),   //
                                   mt(8, {})})}),
                     mt(9, {mt(10, {mt(11, {}),   //
                                    mt(12, {})}), //
                            mt(13, {mt(14, {}),   //
                                    mt(15, {})})}),
                 });

  vector<int> expected_leaves{4, 5, 7, 8, 11, 12, 14, 15};

  BOOST_TEST(expected_leaves == get_leaves_list(t));
}

BOOST_AUTO_TEST_CASE(test_ziptree) {
  auto t = mt(1, {mt(2, {}),                     //
                  mt(3, {mt(4, {}), mt(5, {})}), //
                  mt(6, {mt(7, {mt(8, {})})})});

  using T = std::tuple<int, int, int>;
  auto tripled_expected = mt(
      T(1, 1, 1), {mt(T(2, 2, 2), {}),                                       //
                   mt(T(3, 3, 3), {mt(T(4, 4, 4), {}), mt(T(5, 5, 5), {})}), //
                   mt(T(6, 6, 6), {mt(T(7, 7, 7), {mt(T(8, 8, 8), {})})})});

  auto tripled = ziptree(t, t, t);

  bool check = *tripled == *tripled_expected;
  BOOST_TEST(check);
}

BOOST_AUTO_TEST_CASE(test_nodemap) {

  TreeP<int> t = mt(1, {mt(2, {}),                     //
                        mt(3, {mt(4, {}), mt(5, {})}), //
                        mt(6, {mt(7, {mt(8, {})})})});

  std::function<std::string(int)> f = [](int i) {
    std::stringstream ss;
    ss << "N: ";
    ss << i * 3;
    return ss.str();
  };

  auto s = [](const char *st) { return std::string(st); };
  auto expt =
      mt(s("N: 3"), {mt(s("N: 6"), {}),                                       //
                     mt(s("N: 9"), {mt(s("N: 12"), {}), mt(s("N: 15"), {})}), //
                     mt(s("N: 18"), {mt(s("N: 21"), {mt(s("N: 24"), {})})})});

  auto newt = nodemap(t, f);
  BOOST_TEST(*expt == *newt);
}

BOOST_AUTO_TEST_CASE(test_collapse_level) {

  auto t = mt(1, {
                     mt(2, {mt(3, {mt(4, {}),   //
                                   mt(5, {})}), //
                            mt(6, {mt(7, {}),   //
                                   mt(8, {})})}),
                     mt(9, {mt(10, {mt(11, {}),   //
                                    mt(12, {})}), //
                            mt(13, {mt(14, {}),   //
                                    mt(15, {})})}),
                 });

  auto tcollapsed0_exp = mt(1, {
                                   mt(3, {mt(4, {}),     //
                                          mt(5, {})}),   //
                                   mt(10, {mt(11, {}),   //
                                           mt(12, {})}), //
                               });

  auto tcollapsed0 = collapse_level(t,                       //
                                    1 /*level to collapse*/, //
                                    0 /*child to replace*/);

  BOOST_TEST(*tcollapsed0_exp == *tcollapsed0);
}

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
BOOST_AUTO_TEST_CASE(test_bring_level_on_top_noop) {
  auto t = mt(1, {
                     mt(2, {mt(3, {mt(4, {}), //
                                   mt(5, {})})}),
                     mt(9, {mt(3, {mt(11, {}), //
                                   mt(12, {})})}),
                 });

  auto tcopy = bring_level_on_top(t, 0);
  BOOST_TEST(*t == *tcopy);
}

BOOST_AUTO_TEST_CASE(test_bring_level_on_top) {
  auto t = mt(1, {
                     mt(2, {mt(3, {mt(4, {}), //
                                   mt(5, {})})}),
                     mt(9, {mt(3, {mt(11, {}), //
                                   mt(12, {})})}),
                 });

  auto transf_expected = mt(3, {mt(1, {mt(2, {mt(4, {})}), //
                                       mt(9, {mt(11, {})})}),
                                mt(1, {mt(2, {mt(5, {})}), //
                                       mt(9, {mt(12, {})})})});

  auto transf = bring_level_on_top(t, 2);

  BOOST_TEST(*transf_expected == *transf);
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
BOOST_AUTO_TEST_CASE(test_bring_level_on_top_wrong_nodes) {
  auto t = mt(1, {
                     mt(2, {mt(3, {mt(4, {}), //
                                   mt(5, {})})}),
                     mt(9, {mt(0, {mt(11, {}), //
                                   mt(12, {})})}),
                 });
  BOOST_CHECK_THROW(bring_level_on_top(t, 2), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_bring_level_on_top_wrong_children) {
  auto t = mt(1, {
                     mt(2, {mt(3, {mt(4, {}), //
                                   mt(5, {})})}),
                     mt(9, {mt(3, {mt(11, {})})}),
                 });
  BOOST_CHECK_THROW(bring_level_on_top(t, 2), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_swap_levels_noop) {

  auto t = mt(1, {mt(2, {mt(3, {mt(5, {}),     //
                                mt(6, {})})}), //
                  mt(4, {mt(3, {mt(5, {}),     //
                                mt(6, {})})})});
  vector<int> new_level_ordering{0, 1, 2};

  auto newt = swap_levels(t, new_level_ordering);

  BOOST_TEST(*newt == *t);
}

BOOST_AUTO_TEST_CASE(test_swap_levels) {

  auto t = mt(1, {mt(2, {mt(3, {mt(5, {}),     //
                                mt(6, {})})}), //
                  mt(4, {mt(3, {mt(7, {}),     //
                                mt(8, {})})})});
  vector<int> new_level_ordering{2, 0, 1};

  auto newt_exp = mt(3, {mt(1, {mt(2, {mt(5, {})}),   //
                                mt(4, {mt(7, {})})}), //
                         mt(1, {mt(2, {mt(6, {})}),   //
                                mt(4, {mt(8, {})})})});

  auto newt = swap_levels(t, new_level_ordering);

  BOOST_TEST(*newt == *newt_exp);
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
BOOST_AUTO_TEST_CASE(test_swap_levels_non_cylic) {

  auto t = mt(1, {mt(2, {mt(3, {mt(5, {}),     //
                                mt(6, {})})}), //
                  mt(2, {mt(3, {mt(7, {}),     //
                                mt(8, {})})})});
  vector<int> new_level_ordering{2, 1, 0};

  auto newt_exp = mt(3, {mt(2, {mt(1, {mt(5, {}),     //
                                       mt(7, {})})}), //
                         mt(2, {mt(1, {mt(6, {}),     //
                                       mt(8, {})})})});

  auto newt = swap_levels(t, new_level_ordering);

  BOOST_TEST(*newt == *newt_exp);
}

BOOST_AUTO_TEST_CASE(test_first_nodes_list) {
  auto t = mt(1, {mt(2, {mt(3, {mt(5, {}),     //
                                mt(6, {})})}), //
                  mt(4, {mt(3, {mt(7, {}),     //
                                mt(8, {})})})});

  vector<int> first_nodes_expected{1, 2, 3, 5};

  vector<int> first_nodes = first_nodes_list(t);

  BOOST_TEST(first_nodes == first_nodes_expected);
}

BOOST_AUTO_TEST_CASE(test_depth_first_flatten) {
  auto t = mt(1, {mt(2, {mt(3, {mt(5, {}),     //
                                mt(6, {})})}), //
                  mt(4, {mt(3, {mt(7, {}),     //
                                mt(8, {})})})});

  vector<int> dfflat_expected{1, 2, 3, 5, 6, 4, 3, 7, 8};
  vector<int> dfflat = depth_first_flatten(t);

  BOOST_TEST(dfflat_expected == dfflat);
}

BOOST_AUTO_TEST_SUITE_END()
