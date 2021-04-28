
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

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

BOOST_AUTO_TEST_SUITE_END()
