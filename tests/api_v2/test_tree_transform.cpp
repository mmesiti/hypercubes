#include "api_v2/tree_transform.hpp"
#include "exceptions/exceptions.hpp"
#include "trees/tree.hpp"
#include "trees/tree_data_structure.hpp"
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

using namespace hypercubes::slow::internals;

BOOST_AUTO_TEST_SUITE(test_tree_transform)

BOOST_AUTO_TEST_CASE(test_generate_flat_level) {
  auto t = generate_flat_level(3);
  auto leaf = mtkv(true, {});
  auto texp = mtkv(false, {{{0}, leaf}, //
                           {{1}, leaf}, //
                           {{2}, leaf}});
  BOOST_TEST(*texp == *t);
}

BOOST_AUTO_TEST_CASE(test_tree_product1) {
  auto t2 = generate_flat_level(2);
  auto t = tree_product({t2});
  BOOST_TEST(*t == *t2);
}

BOOST_AUTO_TEST_CASE(test_tree_product2) {
  auto t2 = generate_flat_level(2);
  auto t3 = generate_flat_level(3);
  auto t6 = tree_product({t2, t3});
  auto t6exp = mtkv(false, {{{0}, t3}, //
                            {{1}, t3}});
  BOOST_TEST(*t6 == *t6exp);
}

BOOST_AUTO_TEST_CASE(test_tree_product3) {
  auto t2 = generate_flat_level(2);
  auto t3 = generate_flat_level(3);
  auto _t6 = tree_product({t3, t2});
  auto t12 = tree_product({t2, t3, t2});
  auto t12exp = mtkv(false, {{{0}, _t6}, //
                             {{1}, _t6}});
  BOOST_TEST(*t12 == *t12exp);
}

BOOST_AUTO_TEST_CASE(test_tree_sum) {
  auto t2 = generate_flat_level(2);
  auto t3 = generate_flat_level(3);
  auto t5 = tree_sum({t2, t3});
  auto t5exp = mtkv(false, {{{0}, t2}, //
                            {{1}, t3}});
  BOOST_TEST(*t5 == *t5exp);
}

BOOST_AUTO_TEST_CASE(test_generate_nd_tree) {

  auto t = generate_nd_tree({2, 3});
  auto expected = mtkv(false, {{{0},
                                mtkv(false, {{{0}, mtkv(true, {})},    //
                                             {{1}, mtkv(true, {})},    //
                                             {{2}, mtkv(true, {})}})}, //
                               {{1},
                                mtkv(false, {{{0}, mtkv(true, {})},      //
                                             {{1}, mtkv(true, {})},      //
                                             {{2}, mtkv(true, {})}})}}); //

  BOOST_TEST(*t == *expected);
}

BOOST_AUTO_TEST_CASE(test_partition_q_dimension_0_no_rest) {
  auto t = generate_nd_tree({6});
  auto t_partitioned = q(t, 0, 3);
  auto t_partitioned_expected =
      mtkv(false, {{{},
                    mtkv(false, {{{0}, mtkv(true, {})},    //
                                 {{1}, mtkv(true, {})}})}, //
                   {{},
                    mtkv(false, {{{2}, mtkv(true, {})},    //
                                 {{3}, mtkv(true, {})}})}, //
                   {{},
                    mtkv(false, {{{4}, mtkv(true, {})}, //
                                 {{5}, mtkv(true, {})}})}});
  BOOST_TEST(*t_partitioned == *t_partitioned_expected);
}
BOOST_AUTO_TEST_CASE(test_partition_q_dimension_0_with_rest) {
  auto t = generate_nd_tree({5});
  auto t_partitioned = q(t, 0, 2);
  auto t_partitioned_expected =
      mtkv(false, {{{},
                    mtkv(false, {{{0}, mtkv(true, {})},    //
                                 {{1}, mtkv(true, {})},    //
                                 {{2}, mtkv(true, {})}})}, //
                   {{},
                    mtkv(false, {{{3}, mtkv(true, {})}, //
                                 {{4}, mtkv(true, {})}})}});
  BOOST_TEST(*t_partitioned == *t_partitioned_expected);
}

BOOST_AUTO_TEST_CASE(test_partition_q_dimension_1_with_rest) {
  auto t = generate_nd_tree({2, 5});
  auto t_partitioned = q(t, 1, 2);
  auto subtree = mtkv(false, {{{},
                               mtkv(false, {{{0}, mtkv(true, {})},    //
                                            {{1}, mtkv(true, {})},    //
                                            {{2}, mtkv(true, {})}})}, //
                              {{},
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
      mtkv(false, {{{},
                    mtkv(false, {{{0}, mtkv(true, {})},    //
                                 {{1}, mtkv(true, {})}})}, //
                   {{},
                    mtkv(false, {{{2}, mtkv(true, {})},    //
                                 {{3}, mtkv(true, {})},    //
                                 {{4}, mtkv(true, {})}})}, //
                   {{},
                    mtkv(false, {{{5}, mtkv(true, {})}, //
                                 {{6}, mtkv(true, {})}})}});

  BOOST_TEST(*t_partitioned == *t_partitioned_expected);
}
BOOST_AUTO_TEST_CASE(test_bb_dimension_1) {
  auto t = generate_nd_tree({3, 7});
  auto t_partitioned = bb(t, 1, 2);
  auto subtree = mtkv(false, {{{},
                               mtkv(false, {{{0}, mtkv(true, {})}, //
                                            {{1}, mtkv(true, {})}})},
                              //
                              {{},
                               mtkv(false, {{{2}, mtkv(true, {})}, //
                                            {{3}, mtkv(true, {})}, //
                                            {{4}, mtkv(true, {})}})},
                              //
                              {{},
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

BOOST_AUTO_TEST_CASE(test_index_pullback_id) {
  auto t = generate_nd_tree({2, 2, 2});
  vector<int> in{0, 1, 0};
  auto out = index_pullback(t, in)[0];
  BOOST_TEST(in == out);
}

BOOST_AUTO_TEST_CASE(test_index_pullback_complex) {
  auto subtree = mtkv(false, {{{10}, mtkv(false, {})},                      //
                              {{11}, mtkv(false, {{{6}, mtkv(true, {})}})}, //
                              {{12}, mtkv(false, {{{8}, mtkv(true, {})}})}, //
                              {{13}, mtkv(false, {})}});                    //
  auto t0 = mtkv(false, {{{40}, subtree},                                   //
                         {{21}, subtree}});

  vector<int> in{0, 2, 0};
  vector<int> out_expected{40, 12, 8};
  auto out = index_pullback(t0, in)[0];
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_expected.begin(), out_expected.end());
}

BOOST_AUTO_TEST_CASE(test_index_pushforward_id) {
  auto t = generate_nd_tree({2, 2, 2});
  vector<int> in{0, 1, 0};
  auto out = index_pushforward(t, in);
  BOOST_TEST(vector<vector<int>>{in} == out);
}

BOOST_AUTO_TEST_CASE(test_index_pushforward_complex) {
  auto subtree = mtkv(false, {{{10}, mtkv(false, {})},                      //
                              {{11}, mtkv(false, {{{6}, mtkv(true, {})}})}, //
                              {{12}, mtkv(false, {{{8}, mtkv(true, {})}})}, //
                              {{13}, mtkv(false, {})}});                    //
  auto t0 = mtkv(false, {{{40}, subtree},                                   //
                         {{21}, subtree}});

  vector<int> in{40, 12, 8};
  vector<int> out_expected{0, 2, 0};
  auto out = index_pushforward(t0, in)[0];
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_expected.begin(), out_expected.end());
}

BOOST_AUTO_TEST_CASE(test_index_pullback_and_pushforward) {
  /// Pushforward is the opposite of pullback.
  auto subtree = mtkv(false, {{{10}, mtkv(false, {})},                      //
                              {{11}, mtkv(false, {{{6}, mtkv(true, {})}})}, //
                              {{12}, mtkv(false, {{{8}, mtkv(true, {})}})}, //
                              {{13}, mtkv(false, {})}});                    //
  auto t0 = mtkv(false, {{{40}, subtree},                                   //
                         {{21}, subtree}});
  {
    vector<int> in{0, 2, 0};
    auto pullback = index_pullback(t0, in)[0];
    auto pushforward = index_pushforward(t0, pullback)[0];
    BOOST_CHECK_EQUAL_COLLECTIONS(in.begin(), in.end(), //
                                  pushforward.begin(), pushforward.end());
  }
  /// In both ways!
  {
    vector<int> in{40, 11, 6};
    auto pushforward = index_pushforward(t0, in)[0];
    auto pullback = index_pullback(t0, pushforward)[0];
    BOOST_CHECK_EQUAL_COLLECTIONS(in.begin(), in.end(), //
                                  pullback.begin(), pullback.end());
  }
}

BOOST_AUTO_TEST_CASE(test_index_pullback_throws) {
  auto t = generate_nd_tree({2, 4, 4});
  BOOST_CHECK_THROW(index_pullback(t, {1, 4, 3}), KeyNotFoundError);
}

BOOST_AUTO_TEST_CASE(test_index_pullback_throws_with_level_below_error) {
  /// in this case, the index out of bound is above the level
  /// at which keys are considered.
  auto t = generate_nd_tree({2, 4, 4});
  BOOST_CHECK_THROW(index_pullback(t, {1, 4, 3}), KeyNotFoundError);
}

BOOST_AUTO_TEST_CASE(test_index_pushforward_noresults) {
  auto t = generate_nd_tree({2, 4, 4});
  auto res = index_pushforward(t, {1, 4, 3});
  BOOST_TEST(res.size() == 0);
}

BOOST_AUTO_TEST_CASE(test_index_pullback_q) {
  auto t0 = generate_nd_tree({2, 4, 4});
  int level = 1;
  auto t1 = q(t0, level, 2);
  vector<int> in{0, 1, 1, 2};
  vector<int> out_exp{0, 3, 2};
  auto out = index_pullback(t1, in)[0];
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}

BOOST_AUTO_TEST_CASE(test_index_pushforward_q) {
  auto t0 = generate_nd_tree({2, 4, 4});
  int level = 1;
  auto t1 = q(t0, level, 2);
  vector<int> in{0, 3, 2};
  vector<int> out_exp{0, 1, 1, 2};
  auto outs = index_pushforward(t1, in);
  BOOST_TEST(outs.size() == 1);
  auto out = outs[0];
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}

BOOST_AUTO_TEST_CASE(test_index_pullback_inverse_tree) {
  auto t0 = generate_nd_tree({2, 4, 4});
  int level = 1;
  auto t1 = q(t0, level, 2);
  auto t2 = flatten(t1, level, level + 2); // the inverse tree

  vector<int> in2{0, 3, 2};
  vector<int> out2_exp{0, 1, 1, 2};
  vector<int> out1_exp{0, 3, 2}; // = in2;

  auto out2 = index_pullback(t2, in2)[0];
  auto out1 = index_pullback(t1, out2)[0];
  BOOST_CHECK_EQUAL_COLLECTIONS(out2.begin(), out2.end(), //
                                out2_exp.begin(), out2_exp.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(out1.begin(), out1.end(), //
                                out1_exp.begin(), out1_exp.end());
}

BOOST_AUTO_TEST_CASE(test_index_pushforward_both_ways) {
  auto t0 = generate_nd_tree({2, 4, 4});
  int level = 1;
  auto t1 = q(t0, level, 2);
  auto t2 = flatten(t1, level, level + 2); // the inverse tree

  vector<int> in2{0, 3, 2};
  vector<int> out2_exp{0, 1, 1, 2};
  vector<int> out1_exp{0, 3, 2}; // = in2;

  auto out2 = index_pushforward(t1, in2)[0];
  auto out1 = index_pushforward(t2, out2)[0];
  BOOST_CHECK_EQUAL_COLLECTIONS(out2.begin(), out2.end(), //
                                out2_exp.begin(), out2_exp.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(out1.begin(), out1.end(), //
                                out1_exp.begin(), out1_exp.end());
}

BOOST_AUTO_TEST_CASE(test_remap_level) {
  auto subtree = mtkv(false, {{{10}, mtkv(false, {})},                      //
                              {{11}, mtkv(false, {{{6}, mtkv(true, {})}})}, //
                              {{12}, mtkv(false, {{{8}, mtkv(true, {})}})}, //
                              {{13}, mtkv(false, {})}});                    //
  auto t0 = mtkv(false, {{{0}, subtree},                                    //
                         {{1}, subtree}});
  vector<int> remapping{3, 0, 2, 1};
  auto t1 = remap_level(t0, 1, remapping);
  auto subtree1_exp =
      mtkv(false, {{{3}, mtkv(false, {})},                        //
                   {{0}, mtkv(false, {})},                        //
                   {{2}, mtkv(false, {{{0}, mtkv(true, {})}})},   //
                   {{1}, mtkv(false, {{{0}, mtkv(true, {})}})}}); //
  auto t1_exp = mtkv(false, {{{0}, subtree1_exp},                 //
                             {{1}, subtree1_exp}});
  BOOST_TEST(*t1 == *t1_exp);
}

BOOST_AUTO_TEST_CASE(test_remap_level_pullback_index) {
  auto subtree = mtkv(false, {{{10}, mtkv(false, {})},                      //
                              {{11}, mtkv(false, {{{6}, mtkv(true, {})}})}, //
                              {{12}, mtkv(false, {{{8}, mtkv(true, {})}})}, //
                              {{13}, mtkv(false, {})}});                    //
  auto t0 = mtkv(false, {{{0}, subtree},                                    //
                         {{1}, subtree}});
  vector<int> remapping{3, 0, 2, 1};
  int level = 1;
  auto t1 = remap_level(t0, level, remapping);

  vector<int> in{0, 3, 0};
  auto out = index_pullback(t1, in)[0];
  vector<int> out_expected{0, 1, 0};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_expected.begin(), out_expected.end());
}
BOOST_AUTO_TEST_CASE(test_remap_level_pushforward_index_multiple) {
  auto subtree = mtkv(false, {{{10}, mtkv(false, {})},                      //
                              {{11}, mtkv(false, {{{6}, mtkv(true, {})}})}, //
                              {{12}, mtkv(false, {{{8}, mtkv(true, {})}})}, //
                              {{13}, mtkv(false, {})}});                    //
  auto t0 = mtkv(false, {{{0}, subtree},                                    //
                         {{1}, subtree}});
  vector<int> remapping{3, 2, 1, 2};
  int level = 1;
  auto t1 = remap_level(t0, level, remapping);

  vector<int> in{0, 2, 0};
  auto outs = index_pushforward(t1, in);
  vector<vector<int>> outs_expected{{0, 1, 0}, {0, 3, 0}};
  BOOST_CHECK_EQUAL_COLLECTIONS(outs.begin(), outs.end(), //
                                outs_expected.begin(), outs_expected.end());
}

BOOST_AUTO_TEST_CASE(test_flatten_pullback) {
  auto t = generate_nd_tree({2, 2, 2});
  auto t_collapsed = flatten(t, 0, 2);

  auto subtree = generate_nd_tree({2});
  auto t_collapsed_expected = mtkv(false, {{{0, 0}, subtree}, //
                                           {{0, 1}, subtree}, //
                                           {{1, 0}, subtree}, //
                                           {{1, 1}, subtree}});

  auto out = index_pullback(t_collapsed, {2, 0})[0];
  decltype(out) out_expected{1, 0, 0};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_expected.begin(), out_expected.end());
}
BOOST_AUTO_TEST_CASE(test_flatten_pushforward) {
  auto t = generate_nd_tree({2, 2, 2});
  auto t_collapsed = flatten(t, 0, 2);

  auto subtree = generate_nd_tree({2});
  auto t_collapsed_expected = mtkv(false, {{{0, 0}, subtree}, //
                                           {{0, 1}, subtree}, //
                                           {{1, 0}, subtree}, //
                                           {{1, 1}, subtree}});
  auto out = index_pushforward(t_collapsed, {1, 0, 0})[0];
  decltype(out) out_expected{2, 0};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_expected.begin(), out_expected.end());
}

// TODO: Test that instead of crashes meaningful exceptions are thrown
//       (is this a good thing?)
BOOST_AUTO_TEST_CASE(test_swap_levels_by_key) {

  auto t = mtkv(1, {{{10},
                     mtkv(2, {{{12},
                               mtkv(3, {{{14}, mtkv(5, {})},       //
                                        {{15}, mtkv(6, {})}})}})}, //
                    {{11},
                     mtkv(4, {{{13},
                               mtkv(3, {{{14}, mtkv(7, {})}, //
                                        {{15}, mtkv(8, {})}})}})}});
  vector<int> new_level_ordering{2, 0, 1};

  auto newt_exp =
      mtkv(3, {{{14},
                mtkv(1, {{{10}, mtkv(2, {{{12}, mtkv(5, {})}})},    //
                         {{11}, mtkv(4, {{{13}, mtkv(7, {})}})}})}, //
               {{15},
                mtkv(1, {{{10}, mtkv(2, {{{12}, mtkv(6, {})}})}, //
                         {{11}, mtkv(4, {{{13}, mtkv(8, {})}})}})}});

  auto newt = swap_levels(t, new_level_ordering);

  BOOST_TEST(*newt == *newt_exp);
}

BOOST_AUTO_TEST_SUITE_END()
