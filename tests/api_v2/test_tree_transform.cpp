#include "api_v2/tree_transform.hpp"
#include "trees/tree.hpp"
#include "trees/tree_data_structure.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

using namespace hypercubes::slow::internals;

BOOST_AUTO_TEST_SUITE(test_tree_transform)

BOOST_AUTO_TEST_CASE(test_generate_nd_tree) {

  auto t = generate_nd_tree({2, 3});
  auto expected = mtkv(false, {{{NOKEY},
                                mtkv(false, {{{NOKEY}, mtkv(true, {})},    //
                                             {{NOKEY}, mtkv(true, {})},    //
                                             {{NOKEY}, mtkv(true, {})}})}, //
                               {{NOKEY},
                                mtkv(false, {{{NOKEY}, mtkv(true, {})},      //
                                             {{NOKEY}, mtkv(true, {})},      //
                                             {{NOKEY}, mtkv(true, {})}})}}); //

  BOOST_TEST(*t == *expected);
}

BOOST_AUTO_TEST_CASE(test_partition_q_dimension_0_no_rest) {
  auto t = generate_nd_tree({6});
  auto t_partitioned = q(t, 0, 3);
  auto t_partitioned_expected =
      mtkv(false, {{{NOKEY},
                    mtkv(false, {{{0}, mtkv(true, {})},    //
                                 {{1}, mtkv(true, {})}})}, //
                   {{NOKEY},
                    mtkv(false, {{{2}, mtkv(true, {})},    //
                                 {{3}, mtkv(true, {})}})}, //
                   {{NOKEY},
                    mtkv(false, {{{4}, mtkv(true, {})}, //
                                 {{5}, mtkv(true, {})}})}});
  BOOST_TEST(*t_partitioned == *t_partitioned_expected);
}
BOOST_AUTO_TEST_CASE(test_partition_q_dimension_0_with_rest) {
  auto t = generate_nd_tree({5});
  auto t_partitioned = q(t, 0, 2);
  auto t_partitioned_expected =
      mtkv(false, {{{NOKEY},
                    mtkv(false, {{{0}, mtkv(true, {})},    //
                                 {{1}, mtkv(true, {})},    //
                                 {{2}, mtkv(true, {})}})}, //
                   {{NOKEY},
                    mtkv(false, {{{3}, mtkv(true, {})}, //
                                 {{4}, mtkv(true, {})}})}});
  BOOST_TEST(*t_partitioned == *t_partitioned_expected);
}

BOOST_AUTO_TEST_CASE(test_partition_q_dimension_1_with_rest) {
  auto t = generate_nd_tree({2, 5});
  auto t_partitioned = q(t, 1, 2);
  auto subtree = mtkv(false, {{{NOKEY},
                               mtkv(false, {{{0}, mtkv(true, {})},    //
                                            {{1}, mtkv(true, {})},    //
                                            {{2}, mtkv(true, {})}})}, //
                              {{NOKEY},
                               mtkv(false, {{{3}, mtkv(true, {})}, //
                                            {{4}, mtkv(true, {})}})}});

  auto t_partitioned_expected = mtkv(false, {{{0}, subtree}, //
                                             {{1}, subtree}});
  BOOST_TEST(*t_partitioned == *t_partitioned_expected);
}

BOOST_AUTO_TEST_CASE(test_partition_q_dimension_0_as_equal_as_possible) {
  auto t = generate_nd_tree({23});
  auto t_partitioned = q(t, 0, 19);
  BOOST_TEST(t_partitioned->children.size() == 19);
  for (auto c : t_partitioned->children) {
    bool success =
        c.second->children.size() == 1 or c.second->children.size() == 2;
    BOOST_TEST(success);
  }
}

BOOST_AUTO_TEST_CASE(test_bb_dimension_0) {
  auto t = generate_nd_tree({7});
  auto t_partitioned = bb(t, 0, 2);
  auto t_partitioned_expected =
      mtkv(false, {{{NOKEY},
                    mtkv(false, {{{0}, mtkv(true, {})},    //
                                 {{1}, mtkv(true, {})}})}, //
                   {{NOKEY},
                    mtkv(false, {{{2}, mtkv(true, {})},    //
                                 {{3}, mtkv(true, {})},    //
                                 {{4}, mtkv(true, {})}})}, //
                   {{NOKEY},
                    mtkv(false, {{{5}, mtkv(true, {})}, //
                                 {{6}, mtkv(true, {})}})}});

  BOOST_TEST(*t_partitioned == *t_partitioned_expected);
}
BOOST_AUTO_TEST_CASE(test_bb_dimension_1) {
  auto t = generate_nd_tree({3, 7});
  auto t_partitioned = bb(t, 1, 2);
  auto subtree = mtkv(false, {{{NOKEY},
                               mtkv(false, {{{0}, mtkv(true, {})}, //
                                            {{1}, mtkv(true, {})}})},
                              //
                              {{NOKEY},
                               mtkv(false, {{{2}, mtkv(true, {})}, //
                                            {{3}, mtkv(true, {})}, //
                                            {{4}, mtkv(true, {})}})},
                              //
                              {{NOKEY},
                               mtkv(false, {{{5}, mtkv(true, {})}, //
                                            {{6}, mtkv(true, {})}})}});

  auto t_partitioned_expected = mtkv(false, {{{0}, subtree}, //
                                             {{1}, subtree}, //
                                             {{2}, subtree}});
  BOOST_TEST(*t_partitioned == *t_partitioned_expected);
}

BOOST_AUTO_TEST_CASE(test_flatten_levels_2d) {
  auto t = generate_nd_tree({2, 2});
  auto t_collapsed = flatten(t, 0, 2);
  auto t_collapsed_expected = mtkv(false, {{{0, 0}, mtkv(true, {})}, //
                                           {{0, 1}, mtkv(true, {})}, //
                                           {{1, 0}, mtkv(true, {})}, //
                                           {{1, 1}, mtkv(true, {})}});
  BOOST_TEST(*t_collapsed == *t_collapsed_expected);
}

BOOST_AUTO_TEST_CASE(test_flatten_levels_3d_13) {
  auto t = generate_nd_tree({2, 2, 2});
  auto t_collapsed = flatten(t, 1, 3);
  auto subtree = mtkv(false, {{{0, 0}, mtkv(true, {})}, //
                              {{0, 1}, mtkv(true, {})}, //
                              {{1, 0}, mtkv(true, {})}, //
                              {{1, 1}, mtkv(true, {})}});
  auto t_collapsed_expected = mtkv(false, {{{0}, subtree}, {{1}, subtree}});
  BOOST_TEST(*t_collapsed == *t_collapsed_expected);
}

BOOST_AUTO_TEST_CASE(test_flatten_levels_3d_02) {
  auto t = generate_nd_tree({2, 2, 2});
  auto t_collapsed = flatten(t, 0, 2);

  auto subtree = generate_nd_tree({2});
  auto t_collapsed_expected = mtkv(false, {{{0, 0}, subtree}, //
                                           {{0, 1}, subtree}, //
                                           {{1, 0}, subtree}, //
                                           {{1, 1}, subtree}});
  BOOST_TEST(*t_collapsed == *t_collapsed_expected);
}

BOOST_AUTO_TEST_CASE(test_eo_naive_1d_even) {
  auto t = generate_nd_tree({4});
  auto t_numbered = q(t, 0, 1);
  auto t_sub_expected = mtkv(false, {{{0},
                                      mtkv(false, {{{0}, mtkv(true, {})},    //
                                                   {{2}, mtkv(true, {})}})}, //
                                     {{1},
                                      mtkv(false, {{{1}, mtkv(true, {})}, //
                                                   {{3}, mtkv(true, {})}})}});

  auto t_partitioned = eo_naive(t_numbered, 1);
  auto t_expected = mtkv(false, {{{0}, t_sub_expected}});
  BOOST_TEST(*t_partitioned == *t_expected);
}
BOOST_AUTO_TEST_CASE(test_eo_naive_1d_odd) {
  auto t = generate_nd_tree({5});
  auto t_numbered = q(t, 0, 1);
  auto t_sub_expected = mtkv(false, {{{0},
                                      mtkv(false, {{{0}, mtkv(true, {})},    //
                                                   {{2}, mtkv(true, {})},    //
                                                   {{4}, mtkv(true, {})}})}, //
                                     {{1},
                                      mtkv(false, {{{1}, mtkv(true, {})}, //
                                                   {{3}, mtkv(true, {})}})}});

  auto t_partitioned = eo_naive(t_numbered, 1);
  auto t_expected = mtkv(false, {{{0}, t_sub_expected}});
  BOOST_TEST(*t_partitioned == *t_expected);
}

BOOST_AUTO_TEST_SUITE_END()
