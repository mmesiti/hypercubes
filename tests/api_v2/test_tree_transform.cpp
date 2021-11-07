#include "api_v2/tree_transform.hpp"
#include "exceptions/exceptions.hpp"
#include "trees/tree.hpp"
#include "trees/tree_data_structure.hpp"
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

using namespace hypercubes::slow::internals;
template <class A, class B> std::pair<A, B> mp(A a, B b) {
  return std::make_pair(a, b);
}

BOOST_AUTO_TEST_SUITE(test_tree_transform)

BOOST_AUTO_TEST_CASE(test_generate_flat_level) {
  TreeFactory<bool> f;
  auto t = f.generate_flat_level(3);
  auto leaf = mtkv(true, {});
  auto texp = mtkv(false, {{{0}, leaf}, //
                           {{1}, leaf}, //
                           {{2}, leaf}});
  BOOST_TEST(*texp == *t);
}

BOOST_AUTO_TEST_CASE(test_tree_product1) {
  TreeFactory<bool> f;
  auto t2 = f.generate_flat_level(2);
  auto t = f.tree_product({t2});
  BOOST_TEST(*t == *t2);
}

BOOST_AUTO_TEST_CASE(test_tree_product2) {
  TreeFactory<bool> f;
  auto t2 = f.generate_flat_level(2);
  auto t3 = f.generate_flat_level(3);
  auto t6 = f.tree_product({t2, t3});
  auto t6exp = mtkv(false, {{{0}, t3}, //
                            {{1}, t3}});
  BOOST_TEST(*t6 == *t6exp);
}

BOOST_AUTO_TEST_CASE(test_tree_product3) {
  TreeFactory<bool> f;
  auto t2 = f.generate_flat_level(2);
  auto t3 = f.generate_flat_level(3);
  auto _t6 = f.tree_product({t3, t2});
  auto t12 = f.tree_product({t2, t3, t2});
  auto t12exp = mtkv(false, {{{0}, _t6}, //
                             {{1}, _t6}});
  BOOST_TEST(*t12 == *t12exp);
}

BOOST_AUTO_TEST_CASE(test_tree_sum) {
  TreeFactory<bool> f;
  auto t2 = f.generate_flat_level(2);
  auto t3 = f.generate_flat_level(3);
  auto t5 = f.tree_sum({t2, t3});
  auto t5exp = mtkv(false, {{{}, t2}, //

                            {{}, t3}});
  BOOST_TEST(*t5 == *t5exp);
}

BOOST_AUTO_TEST_CASE(test_generate_nd_tree) {

  TreeFactory<bool> f;
  auto t = f.generate_nd_tree({2, 3});
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
  TreeFactory<bool> f;
  auto t = f.generate_nd_tree({6});
  auto t_partitioned = f.q(t, 0, 3);
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
  TreeFactory<bool> f;
  auto t = f.generate_nd_tree({5});
  auto t_partitioned = f.q(t, 0, 2);
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
  TreeFactory<bool> f;
  auto t = f.generate_nd_tree({2, 5});
  auto t_partitioned = f.q(t, 1, 2);
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
  TreeFactory<bool> f;
  auto t = f.generate_nd_tree({23});
  auto t_partitioned = f.q(t, 0, 19);
  BOOST_TEST(t_partitioned->children.size() == 19);
  for (auto c : t_partitioned->children) {
    bool success =
        c.second->children.size() == 1 or c.second->children.size() == 2;
    BOOST_TEST(success);
  }
}

BOOST_AUTO_TEST_CASE(test_partition_q_no_empty_trees) {
  TreeFactory<bool> f;
  auto t = f.generate_nd_tree({4});
  auto t_partitioned = f.q(t, 0, 5);
  auto leaf = mtkv(true, {});
  auto t_partitioned_expected = mtkv(false, {{{}, mtkv(false, {{{0}, leaf}})},
                                             {{}, mtkv(false, {{{1}, leaf}})},
                                             {{}, mtkv(false, {{{2}, leaf}})},
                                             {{}, mtkv(false, {{{3}, leaf}})}});

  BOOST_TEST(*t_partitioned == *t_partitioned_expected);
}

BOOST_AUTO_TEST_CASE(test_bb_dimension_0) {
  TreeFactory<bool> f;
  auto t = f.generate_nd_tree({7});
  auto t_partitioned = f.bb(t, 0, 2);
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
  TreeFactory<bool> f;
  auto t = f.generate_nd_tree({3, 7});
  auto t_partitioned = f.bb(t, 1, 2);
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
BOOST_AUTO_TEST_CASE(test_partition_bb_no_empty_trees) {
  TreeFactory<bool> f;
  auto t = f.generate_nd_tree({2});
  auto t_partitioned = f.bb(t, 0, 1);
  auto leaf = mtkv(true, {});
  auto t_partitioned_expected = mtkv(false, {{{}, mtkv(false, {{{0}, leaf}})},
                                             {{}, mtkv(false, {{{1}, leaf}})}});

  BOOST_TEST(*t_partitioned == *t_partitioned_expected);
}

BOOST_AUTO_TEST_CASE(test_flatten_levels_2d) {
  TreeFactory<bool> f;
  auto t = f.generate_nd_tree({2, 2});
  auto t_collapsed = f.flatten(t, 0, 2);
  auto t_collapsed_expected = mtkv(false, {{{0, 0}, mtkv(true, {})}, //
                                           {{0, 1}, mtkv(true, {})}, //
                                           {{1, 0}, mtkv(true, {})}, //
                                           {{1, 1}, mtkv(true, {})}});
  BOOST_TEST(*t_collapsed == *t_collapsed_expected);
}

BOOST_AUTO_TEST_CASE(test_flatten_levels_3d_13) {
  TreeFactory<bool> f;
  auto t = f.generate_nd_tree({2, 2, 2});
  auto t_collapsed = f.flatten(t, 1, 3);
  auto subtree = mtkv(false, {{{0, 0}, mtkv(true, {})}, //
                              {{0, 1}, mtkv(true, {})}, //
                              {{1, 0}, mtkv(true, {})}, //
                              {{1, 1}, mtkv(true, {})}});
  auto t_collapsed_expected = mtkv(false, {{{0}, subtree}, {{1}, subtree}});
  BOOST_TEST(*t_collapsed == *t_collapsed_expected);
}

BOOST_AUTO_TEST_CASE(test_flatten_levels_3d_02) {
  TreeFactory<bool> f;
  auto t = f.generate_nd_tree({2, 2, 2});
  auto t_collapsed = f.flatten(t, 0, 2);

  auto subtree = f.generate_nd_tree({2});
  auto t_collapsed_expected = mtkv(false, {{{0, 0}, subtree}, //
                                           {{0, 1}, subtree}, //
                                           {{1, 0}, subtree}, //
                                           {{1, 1}, subtree}});
  BOOST_TEST(*t_collapsed == *t_collapsed_expected);
}

BOOST_AUTO_TEST_CASE(test_eo_naive_1d_even) {
  TreeFactory<bool> f;
  auto t = f.generate_nd_tree({4});
  auto t_nested = f.q(t, 0, 1);
  auto t_sub_expected = mtkv(false, {{{},
                                      mtkv(false, {{{0}, mtkv(true, {})},    //
                                                   {{2}, mtkv(true, {})}})}, //
                                     {{},
                                      mtkv(false, {{{1}, mtkv(true, {})}, //
                                                   {{3}, mtkv(true, {})}})}});

  auto t_partitioned = f.eo_naive(t_nested, 1);
  auto t_expected = mtkv(false, {{{0}, t_sub_expected}});
  BOOST_TEST(*t_partitioned == *t_expected);
}
BOOST_AUTO_TEST_CASE(test_eo_naive_1d_odd) {
  TreeFactory<bool> f;
  auto t = f.generate_nd_tree({5});
  auto t_nested = f.q(t, 0, 1);
  auto t_sub_expected = mtkv(false, {{{},
                                      mtkv(false, {{{0}, mtkv(true, {})},    //
                                                   {{2}, mtkv(true, {})},    //
                                                   {{4}, mtkv(true, {})}})}, //
                                     {{},
                                      mtkv(false, {{{1}, mtkv(true, {})}, //
                                                   {{3}, mtkv(true, {})}})}});

  auto t_partitioned = f.eo_naive(t_nested, 1);
  auto t_expected = mtkv(false, {{{0}, t_sub_expected}});
  BOOST_TEST(*t_partitioned == *t_expected);
}

BOOST_AUTO_TEST_CASE(test_eo_naive_no_empty_tree) {
  TreeFactory<bool> f;
  auto t = f.generate_nd_tree({1});
  auto t_partitioned = f.eo_naive(t, 0);
  auto leaf = mtkv(true, {});
  auto t_partitioned_expected = mtkv(false, {{{}, mtkv(false, {{{0}, leaf}})}});
  BOOST_TEST(*t_partitioned == *t_partitioned_expected);
}

BOOST_AUTO_TEST_CASE(test_index_pullback_id) {
  TreeFactory<bool> f;
  auto t = f.generate_nd_tree({2, 2, 2});
  vector<int> in{0, 1, 0};
  auto out = index_pullback(t, in);
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
  auto out = index_pullback(t0, in);
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_expected.begin(), out_expected.end());
}

BOOST_AUTO_TEST_CASE(test_index_pushforward_id) {
  TreeFactory<bool> f;
  auto t = f.generate_nd_tree({2, 2, 2});
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
    auto pullback = index_pullback(t0, in);
    auto pushforward = index_pushforward(t0, pullback)[0];
    BOOST_CHECK_EQUAL_COLLECTIONS(in.begin(), in.end(), //
                                  pushforward.begin(), pushforward.end());
  }
  /// In both ways!
  {
    vector<int> in{40, 11, 6};
    auto pushforward = index_pushforward(t0, in)[0];
    auto pullback = index_pullback(t0, pushforward);
    BOOST_CHECK_EQUAL_COLLECTIONS(in.begin(), in.end(), //
                                  pullback.begin(), pullback.end());
  }
}

BOOST_AUTO_TEST_CASE(test_index_pullback_throws) {
  TreeFactory<bool> f;
  auto t = f.generate_nd_tree({2, 4, 4});
  BOOST_CHECK_THROW(index_pullback(t, {1, 4, 3}), KeyNotFoundError);
}

BOOST_AUTO_TEST_CASE(test_index_pullback_throws_negative) {
  TreeFactory<bool> f;
  auto t = f.generate_nd_tree({2, 4, 4});
  BOOST_CHECK_THROW(index_pullback(t, {1, 1, -1}), KeyNotFoundError);
}

BOOST_AUTO_TEST_CASE(test_index_pushforward_noresults) {
  TreeFactory<bool> f;
  auto t = f.generate_nd_tree({2, 4, 4});
  auto res = index_pushforward(t, {1, 4, 3});
  BOOST_TEST(res.size() == 0);
}

BOOST_AUTO_TEST_CASE(test_index_pullback_q) {
  TreeFactory<bool> f;
  auto t0 = f.generate_nd_tree({2, 4, 4});
  int level = 1;
  auto t1 = f.q(t0, level, 2);
  vector<int> in{0, 1, 1, 2};
  vector<int> out_exp{0, 3, 2};
  auto out = index_pullback(t1, in); // because q creates 2 new levels
  BOOST_TEST(out.size() == 3);
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}

BOOST_AUTO_TEST_CASE(test_index_pushforward_q) {
  TreeFactory<bool> f;
  auto t0 = f.generate_nd_tree({2, 4, 4});
  int level = 1;
  auto t1 = f.q(t0, level, 2);
  vector<int> in{0, 3, 2};
  vector<int> out_exp{0, 1, 1, 2};
  auto out = index_pushforward(t1, in)[0];
  BOOST_TEST(out.size() == 4);
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}

BOOST_AUTO_TEST_CASE(test_index_pullback_inverse_tree) {
  TreeFactory<bool> f;
  auto t0 = f.generate_nd_tree({2, 4, 4});
  int level = 1;
  auto t1 = f.q(t0, level, 2);
  auto t2 = f.flatten(t1, level, level + 2); // the inverse tree

  vector<int> in2{0, 3, 2};
  vector<int> out2_exp{0, 1, 1, 2};
  vector<int> out1_exp{0, 3, 2}; // = in2;

  auto out2 = index_pullback(t2, in2);
  auto out1 = index_pullback(t1, out2);
  BOOST_CHECK_EQUAL_COLLECTIONS(out2.begin(), out2.end(), //
                                out2_exp.begin(), out2_exp.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(out1.begin(), out1.end(), //
                                out1_exp.begin(), out1_exp.end());
}

BOOST_AUTO_TEST_CASE(test_index_pushforward_both_ways) {
  TreeFactory<bool> f;
  auto t0 = f.generate_nd_tree({2, 4, 4});
  int level = 1;
  auto t1 = f.q(t0, level, 2);
  auto t2 = f.flatten(t1, level, level + 2); // the inverse tree

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
  TreeFactory<bool> f;
  auto subtree = mtkv(false, {{{10}, mtkv(false, {})},                      //
                              {{11}, mtkv(false, {{{6}, mtkv(true, {})}})}, //
                              {{12}, mtkv(false, {{{8}, mtkv(true, {})}})}, //
                              {{13}, mtkv(false, {})}});                    //
  auto t0 = mtkv(false, {{{0}, subtree},                                    //
                         {{1}, subtree}});
  vector<int> remapping{3, 0, 2, 1};
  auto t1 = f.remap_level(t0, 1, remapping);
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
  TreeFactory<bool> f;
  auto subtree = mtkv(false, {{{10}, mtkv(false, {})},                      //
                              {{11}, mtkv(false, {{{6}, mtkv(true, {})}})}, //
                              {{12}, mtkv(false, {{{8}, mtkv(true, {})}})}, //
                              {{13}, mtkv(false, {})}});                    //
  auto t0 = mtkv(false, {{{0}, subtree},                                    //
                         {{1}, subtree}});
  vector<int> remapping{3, 0, 2, 1};
  int level = 1;
  auto t1 = f.remap_level(t0, level, remapping);

  vector<int> in{0, 3, 0};
  auto out = index_pullback(t1, in);
  vector<int> out_expected{0, 1, 0};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_expected.begin(), out_expected.end());
}
BOOST_AUTO_TEST_CASE(test_remap_level_pushforward_index_multiple) {
  TreeFactory<bool> f;
  auto subtree = mtkv(false, {{{10}, mtkv(false, {})},                      //
                              {{11}, mtkv(false, {{{6}, mtkv(true, {})}})}, //
                              {{12}, mtkv(false, {{{8}, mtkv(true, {})}})}, //
                              {{13}, mtkv(false, {})}});                    //
  auto t0 = mtkv(false, {{{0}, subtree},                                    //
                         {{1}, subtree}});
  vector<int> remapping{3, 2, 1, 2};
  int level = 1;
  auto t1 = f.remap_level(t0, level, remapping);

  vector<int> in{0, 2, 0};
  auto outs = index_pushforward(t1, in);
  vector<vector<int>> outs_expected{{0, 1, 0}, {0, 3, 0}};
  BOOST_CHECK_EQUAL_COLLECTIONS(outs.begin(), outs.end(), //
                                outs_expected.begin(), outs_expected.end());
}

BOOST_AUTO_TEST_CASE(test_flatten_pullback) {
  TreeFactory<bool> f;
  auto t = f.generate_nd_tree({2, 2, 2});
  auto t_collapsed = f.flatten(t, 0, 2);

  auto subtree = f.generate_nd_tree({2});
  auto t_collapsed_expected = mtkv(false, {{{0, 0}, subtree}, //
                                           {{0, 1}, subtree}, //
                                           {{1, 0}, subtree}, //
                                           {{1, 1}, subtree}});
  auto out = index_pullback(t_collapsed, {2, 0});
  decltype(out) out_expected{1, 0, 0};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_expected.begin(), out_expected.end());
}
BOOST_AUTO_TEST_CASE(test_flatten_pushforward) {
  TreeFactory<bool> f;
  auto t = f.generate_nd_tree({2, 2, 2});
  auto t_collapsed = f.flatten(t, 0, 2);

  auto subtree = f.generate_nd_tree({2});
  auto t_collapsed_expected = mtkv(false, {{{0, 0}, subtree}, //
                                           {{0, 1}, subtree}, //
                                           {{1, 0}, subtree}, //
                                           {{1, 1}, subtree}});
  auto out = index_pushforward(t_collapsed, {1, 0, 0})[0];
  decltype(out) out_expected{2, 0};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_expected.begin(), out_expected.end());
}

BOOST_AUTO_TEST_CASE(test_collect_leaves_constructor) {
  TreeFactory<bool> f;
  auto t = f.generate_nd_tree({2, 2});
  auto t_collapsed = f.collect_leaves(t, 0, 4);

  auto leaf = mtkv(true, {});

  auto t_collapsed_expected = mtkv(false, {{{0, 0}, leaf}, //
                                           {{0, 1}, leaf}, //
                                           {{1, 0}, leaf}, //
                                           {{1, 1}, leaf}});
  BOOST_TEST(*t_collapsed == *t_collapsed_expected);
}

BOOST_AUTO_TEST_CASE(test_collect_leaves_constructor_padding) {
  TreeFactory<bool> f;
  auto t = f.generate_nd_tree({2, 3});
  auto t_collapsed = f.collect_leaves(t, 0, 8);

  auto leaf = mtkv(true, {});

  auto nkc = TreeFactory<bool>::no_key_component;

  auto t_collapsed_expected = mtkv(false, {{{0, 0}, leaf}, //
                                           {{0, 1}, leaf}, //
                                           {{0, 2}, leaf}, //
                                           {{1, 0}, leaf}, //
                                           {{1, 1}, leaf}, //
                                           {{1, 2}, leaf}, //
                                           // these do not exist
                                           // in the original tree
                                           {{nkc, nkc}, leaf}, //
                                           {{nkc, nkc}, leaf}});
  BOOST_TEST(*t_collapsed == *t_collapsed_expected);
}

BOOST_AUTO_TEST_CASE(test_collect_leaves_pullback_pad) {
  TreeFactory<bool> f;
  auto t = f.generate_nd_tree({2, 3});
  auto t_collapsed = f.collect_leaves(t, 0, 8);

  auto nkc = TreeFactory<bool>::no_key_component;
  auto out_wrong = index_pullback(t_collapsed, {7});
  vector<int> out_wrong_expected{nkc, nkc};
  BOOST_TEST(out_wrong == out_wrong_expected);
  auto out = index_pullback_pad(t_collapsed, {7});
  BOOST_TEST(out.size() == 0);
}

BOOST_AUTO_TEST_CASE(test_collect_leaves_pushforward) {
  TreeFactory<bool> f;
  auto t = f.generate_nd_tree({2, 3});
  auto t_collapsed = f.collect_leaves(t, 0, 8);

  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 3; ++j) {
      auto out = index_pushforward(t_collapsed, {i, j})[0];
      auto in = index_pullback_pad(t_collapsed, out)[0];
      BOOST_TEST(in == vector<int>({i, j}));
    }
}

// TODO: Test that instead of crashes meaningful exceptions are thrown
//       (is this a good thing?)
BOOST_AUTO_TEST_CASE(test_swap_levels_by_key) {
  TreeFactory<int> f;

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

  auto newt = f.swap_levels(t, new_level_ordering);

  BOOST_TEST(*newt == *newt_exp);
}

BOOST_AUTO_TEST_CASE(test_renumber_children) {
  TreeFactory<int> f;

  auto t = mtkv(1, {{{10},
                     mtkv(2, {{{12},
                               mtkv(3, {{{14}, mtkv(5, {})},       //
                                        {{15}, mtkv(6, {})}})}})}, //
                    {{11},
                     mtkv(4, {{{13},
                               mtkv(3, {{{14}, mtkv(7, {})}, //
                                        {{15}, mtkv(8, {})}})}})}});

  auto expren = mtkv(1, {{{0},
                          mtkv(2, {{{0},
                                    mtkv(3, {{{0}, mtkv(5, {})},       //
                                             {{1}, mtkv(6, {})}})}})}, //
                         {{1},
                          mtkv(4, {{{0},
                                    mtkv(3, {{{0}, mtkv(7, {})}, //
                                             {{1}, mtkv(8, {})}})}})}});
  auto ren = f.renumber_children(t);
  BOOST_TEST(*ren == *expren);
}

BOOST_AUTO_TEST_CASE(test_bring_level_on_top_key) {
  TreeFactory<int> f;
  auto _t =
      mt(mp(0, 1), {mt(mp(20, 2), {mt(mp(22, 3), {mt(mp(24, 4), {}), //
                                                  mt(mp(25, 5), {})})}),
                    mt(mp(21, 9), {mt(mp(23, 3), {mt(mp(24, 11), {}), //
                                                  mt(mp(26, 12), {})})})});

  auto _transf_expected =
      mt(mp(0, 3), {mt(mp(24, 1), {mt(mp(20, 2), {mt(mp(22, 4), {})}), //
                                   mt(mp(21, 9), {mt(mp(23, 11), {})})}),
                    mt(mp(25, 1), {mt(mp(20, 2), {mt(mp(22, 5), {})}), //
                                   mt(mp(21, 9), {})}),                //
                    mt(mp(26, 1), {mt(mp(20, 2), {}),                  //
                                   mt(mp(21, 9), {mt(mp(23, 12), {})})})});
  auto t = pull_keys_up(_t);
  auto transf_expected = pull_keys_up(_transf_expected);

  auto transf = f.bring_level_on_top(t, 2);

  BOOST_TEST(*transf_expected == *transf);
}

BOOST_AUTO_TEST_CASE(test_bring_level_on_top_key_throws) {
  TreeFactory<int> f;
  auto _t =
      mt(mp(0, 1),
         {mt(mp(20, 2), {mt(mp(22, 3 /*different*/), {mt(mp(24, 4), {}), //
                                                      mt(mp(25, 5), {})})}),
          mt(mp(21, 9), {mt(mp(23, 4 /*different*/), {mt(mp(24, 11), {}), //
                                                      mt(mp(26, 12), {})})})});

  auto t = pull_keys_up(_t);
  BOOST_CHECK_THROW(f.bring_level_on_top(t, 2), TreeLevelPermutationError);
}
BOOST_AUTO_TEST_CASE(test_collapse_level_by_key_all_there) {

  TreeFactory<int> f;
  auto _t = mt(mp(0, 1), {mt(mp(3, 2), {mt(mp(5, 3), {mt(mp(1, 4), {}),     //
                                                      mt(mp(2, 5), {})}),   //
                                        mt(mp(6, 6), {mt(mp(3, 7), {}),     //
                                                      mt(mp(4, 8), {})})}), //
                          mt(mp(4, 9), {mt(mp(5, 10), {mt(mp(5, 11), {}),   //
                                                       mt(mp(7, 12), {})}), //
                                        mt(mp(8, 13), {mt(mp(8, 14), {}),   //
                                                       mt(mp(9, 15), {})})})});

  auto _tcollapsed_exp = mt(mp(0, 1), {mt(mp(3, 3), {mt(mp(1, 4), {}),   //
                                                     mt(mp(2, 5), {})}), //
                                       mt(mp(4, 10), {mt(mp(5, 11), {}), //
                                                      mt(mp(7, 12), {})})});

  auto t = pull_keys_up(_t);
  auto tcollapsed_exp = pull_keys_up(_tcollapsed_exp);

  auto tcollapsed = f.collapse_level(t,                       //
                                     1 /*level to collapse*/, //
                                     {5} /*child key to replace*/);

  BOOST_TEST(*tcollapsed_exp == *tcollapsed);
}
BOOST_AUTO_TEST_CASE(test_collapse_level_by_key_missing) {

  TreeFactory<int> f;
  auto _t = mt(mp(0, 1), {mt(mp(3, 2), {mt(mp(5, 3), {mt(mp(1, 4), {}),     //
                                                      mt(mp(2, 5), {})}),   //
                                        mt(mp(6, 6), {mt(mp(3, 7), {}),     //
                                                      mt(mp(4, 8), {})})}), //
                          mt(mp(4, 9), {mt(mp(6, 10), {mt(mp(5, 11), {}),   //
                                                       mt(mp(7, 12), {})}), //
                                        mt(mp(8, 13), {mt(mp(8, 14), {}),   //
                                                       mt(mp(9, 15), {})})})});

  auto _tcollapsed_exp = mt(mp(0, 1), {mt(mp(3, 3), {mt(mp(1, 4), {}), //
                                                     mt(mp(2, 5), {})})});
  auto t = pull_keys_up(_t);
  auto tcollapsed_exp = pull_keys_up(_tcollapsed_exp);

  auto tcollapsed = f.collapse_level(t,                       //
                                     1 /*level to collapse*/, //
                                     {5} /*child key to replace*/);

  BOOST_TEST(*tcollapsed_exp == *tcollapsed);
}

BOOST_AUTO_TEST_SUITE_END()
