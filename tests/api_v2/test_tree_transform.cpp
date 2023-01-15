#include "api_v2/tree_transform.hpp"
#include "exceptions/exceptions.hpp"
#include "geometry/geometry.hpp"
#include "selectors/bool_maybe.hpp"
#include "trees/kvtree_data_structure.hpp"
#include "trees/tree.hpp"
#include "trees/tree_data_structure.hpp"
#include <algorithm>
#include <boost/test/data/test_case.hpp>
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <stdexcept>

using namespace hypercubes::slow::internals;
using hypercubes::slow::BoolM;

using hypercubes::slow::BoundaryCondition;
namespace bdata = boost::unit_test::data;
template <class A, class B> std::pair<A, B> mp(A a, B b) {
  return std::make_pair(a, b);
}

NodeType n(int i) { return (NodeType)i; }

KVTreePv2<NodeType> convert_tree(const KVTreePv2<int> t) {
  decltype(KVTree<NodeType>::children) newchildren;
  std::transform(t->children.begin(), t->children.end(),
                 std::back_inserter(newchildren),
                 [](auto c) { return mp(c.first, convert_tree(c.second)); });
  return mtkv(n(t->n), newchildren);
}

BOOST_AUTO_TEST_SUITE(test_tree_transform)

BOOST_AUTO_TEST_CASE(test_generate_flat_level) {
  TreeFactory f;
  auto t = f.generate_flat_level(3);
  auto leaf = mtkv(LEAF, {});
  auto texp = mtkv(NODE, {{{0}, leaf}, //
                          {{1}, leaf}, //
                          {{2}, leaf}});
  BOOST_TEST(*texp == *t);
}
BOOST_AUTO_TEST_CASE(test_tree_product_1term) {
  TreeFactory f;
  auto t2 = f.generate_flat_level(2);
  auto t = f.tree_product({t2});
  BOOST_TEST(*t == *t2);
}
BOOST_AUTO_TEST_CASE(test_tree_product_2terms) {
  TreeFactory f;
  auto t2 = f.generate_flat_level(2);
  auto t3 = f.generate_flat_level(3);
  auto t6 = f.tree_product({t2, t3});
  auto t6exp = mtkv(NODE, {{{0}, t3}, //
                           {{1}, t3}});
  BOOST_TEST(*t6 == *t6exp);
}
BOOST_AUTO_TEST_CASE(test_tree_product_3terms) {
  TreeFactory f;
  auto t2 = f.generate_flat_level(2);
  auto t3 = f.generate_flat_level(3);
  auto _t6 = f.tree_product({t3, t2});
  auto t12 = f.tree_product({t2, t3, t2});
  auto t12exp = mtkv(NODE, {{{0}, _t6}, //
                            {{1}, _t6}});
  BOOST_TEST(*t12 == *t12exp);
}

BOOST_AUTO_TEST_CASE(test_tree_sum) {
  TreeFactory f;
  auto t2 = f.generate_flat_level(2);
  auto t3 = f.generate_flat_level(3);
  auto t5 = f.tree_sum({t2, t3});
  auto t5exp = mtkv(NODE, {{{}, t2}, //

                           {{}, t3}});
  BOOST_TEST(*t5 == *t5exp);
}

BOOST_AUTO_TEST_CASE(test_generate_nd_tree) {

  TreeFactory f;
  auto t = f.generate_nd_tree({2, 3});
  auto leaf = mtkv(LEAF, {});
  auto expected = mtkv(NODE, {{{0},
                               mtkv(NODE, {{{0}, leaf},    //
                                           {{1}, leaf},    //
                                           {{2}, leaf}})}, //
                              {{1},
                               mtkv(NODE, {{{0}, leaf},      //
                                           {{1}, leaf},      //
                                           {{2}, leaf}})}}); //

  BOOST_TEST(*t == *expected);
}

BOOST_AUTO_TEST_CASE(test_q_dimension_0_no_rest) {
  TreeFactory f;
  auto leaf = mtkv(LEAF, {});
  auto t = f.generate_nd_tree({6});
  auto t_partitioned = f.qh(t, // tree
                            0, // level
                            3, // nparts
                            0, // halo
                            0, // existing halo
                            BoundaryCondition::OPEN);
  auto t_partitioned_expected = mtkv(NODE, {{{},
                                             mtkv(NODE, {{{0}, leaf},    //
                                                         {{1}, leaf}})}, //
                                            {{},
                                             mtkv(NODE, {{{2}, leaf},    //
                                                         {{3}, leaf}})}, //
                                            {{},
                                             mtkv(NODE, {{{4}, leaf}, //
                                                         {{5}, leaf}})}});
  BOOST_TEST(*t_partitioned == *t_partitioned_expected);
}
BOOST_AUTO_TEST_CASE(test_q_dimension_0_with_rest) {
  TreeFactory f;
  auto leaf = mtkv(LEAF, {});
  auto t = f.generate_nd_tree({5});
  auto t_partitioned = f.qh(t, 0, 2, 0, 0, BoundaryCondition::OPEN);
  auto t_partitioned_expected = mtkv(NODE, {{{},
                                             mtkv(NODE, {{{0}, leaf},    //
                                                         {{1}, leaf},    //
                                                         {{2}, leaf}})}, //
                                            {{},
                                             mtkv(NODE, {{{3}, leaf}, //
                                                         {{4}, leaf}})}});

  BOOST_TEST(*t_partitioned == *t_partitioned_expected);
}
BOOST_AUTO_TEST_CASE(test_q_dimension_1_with_rest) {
  TreeFactory f;
  auto leaf = mtkv(LEAF, {});
  auto t = f.generate_nd_tree({2, 5});
  auto t_partitioned = f.qh(t, 1, 2, 0, 0, BoundaryCondition::OPEN);
  auto subtree = mtkv(NODE, {{{},
                              mtkv(NODE, {{{0}, leaf},    //
                                          {{1}, leaf},    //
                                          {{2}, leaf}})}, //
                             {{},
                              mtkv(NODE, {{{3}, leaf}, //
                                          {{4}, leaf}})}});

  auto t_partitioned_expected = mtkv(NODE, {{{0}, subtree}, //
                                            {{1}, subtree}});
  BOOST_TEST(*t_partitioned == *t_partitioned_expected);
}
BOOST_AUTO_TEST_CASE(test_q_dimension_0_as_equal_as_possible) {
  TreeFactory f;
  auto t = f.generate_nd_tree({23});
  auto t_partitioned = f.qh(t, 0, 19, 0, 0, BoundaryCondition::OPEN);
  BOOST_TEST(t_partitioned->children.size() == 19);
  for (auto c : t_partitioned->children) {
    bool success =
        c.second->children.size() == 1 or c.second->children.size() == 2;
    BOOST_TEST(success);
  }
}
BOOST_AUTO_TEST_CASE(test_q_no_empty_trees) {
  TreeFactory f;
  auto leaf = mtkv(LEAF, {});
  auto t = f.generate_nd_tree({4});
  auto t_partitioned = f.qh(t, 0, 5, 0, 0, BoundaryCondition::OPEN);
  auto t_partitioned_expected = mtkv(NODE, {{{}, mtkv(NODE, {{{0}, leaf}})},
                                            {{}, mtkv(NODE, {{{1}, leaf}})},
                                            {{}, mtkv(NODE, {{{2}, leaf}})},
                                            {{}, mtkv(NODE, {{{3}, leaf}})}});

  BOOST_TEST(*t_partitioned == *t_partitioned_expected);
}
BOOST_AUTO_TEST_CASE(test_qhp_dimension_0_norest_no_existing_halo) {
  TreeFactory f;
  auto leaf = mtkv(LEAF, {});
  auto t = f.generate_nd_tree({6});
  auto t_partitioned =
      f.qh(t,                                              // tree
           0,                                              // level
           3,                                              // nparts
           1,                                              // halo
           0,                                              // existing halo
           hypercubes::slow::BoundaryCondition::PERIODIC); // bc
  auto t_partitioned_expected = mtkv(NODE, {{{},
                                             mtkv(NODE, {{{5}, leaf},    //
                                                         {{0}, leaf},    //
                                                         {{1}, leaf},    //
                                                         {{2}, leaf}})}, //
                                            {{},
                                             mtkv(NODE, {{{1}, leaf},    //
                                                         {{2}, leaf},    //
                                                         {{3}, leaf},    //
                                                         {{4}, leaf}})}, //
                                            {{},
                                             mtkv(NODE, {{{3}, leaf}, //
                                                         {{4}, leaf}, //
                                                         {{5}, leaf}, //
                                                         {{0}, leaf}})}});

  BOOST_TEST(*t_partitioned == *t_partitioned_expected);
}
BOOST_AUTO_TEST_CASE(test_qho_dimension_0_norest_no_existing_halo) {
  TreeFactory f;
  auto leaf = mtkv(LEAF, {});
  auto t = f.generate_nd_tree({6});
  auto nkc = TreeFactory::no_key;
  auto t_partitioned = f.qh(t, // tree
                            0, // level
                            3, // nparts
                            1, // halo
                            0, // existing halo
                            hypercubes::slow::BoundaryCondition::OPEN);
  auto t_partitioned_expected = mtkv(NODE, {{{},
                                             mtkv(NODE, {{{nkc}, leaf}, //
                                                         {{0}, leaf},   //
                                                         {{1}, leaf},   //
                                                         {{2}, leaf}})},
                                            {{},
                                             mtkv(NODE, {{{1}, leaf},    //
                                                         {{2}, leaf},    //
                                                         {{3}, leaf},    //
                                                         {{4}, leaf}})}, //
                                            {{},
                                             mtkv(NODE, {{{3}, leaf}, //
                                                         {{4}, leaf}, //
                                                         {{5}, leaf}, //
                                                         {{nkc}, leaf}})}});

  BOOST_TEST(*t_partitioned == *t_partitioned_expected);
}
BOOST_DATA_TEST_CASE(test_qhp_dimension_0_norest_existing_halo,
                     bdata::xrange(2), _bc) {
  // test existing halo case, where BC should not make a difference
  // First partitioning done with periodic bc
  TreeFactory f;
  auto leaf = mtkv(LEAF, {});
  auto t = f.generate_nd_tree({12});
  // First step done with periodic boundary conditions.
  auto t_partitioned_1 =
      f.qh(t,                                              // tree
           0,                                              // level
           2,                                              // nparts
           1,                                              // halo
           0,                                              // existing halo
           hypercubes::slow::BoundaryCondition::PERIODIC); // bc
  auto bc2 = (hypercubes::slow::BoundaryCondition)_bc;
  auto t_partitioned_2 = f.qh(t_partitioned_1, // tree
                              1,               // level
                              3,               // nparts
                              1,               // halo
                              1,               // existing halo
                              bc2);            // bc

  auto t_sub_partitioned_expected = mtkv(NODE, {{{},
                                                 mtkv(NODE, {{{0}, leaf}, //
                                                             {{1}, leaf}, //
                                                             {{2}, leaf}, //
                                                             {{3}, leaf}})},
                                                {{},
                                                 mtkv(NODE, {{{2}, leaf}, //
                                                             {{3}, leaf}, //
                                                             {{4}, leaf}, //
                                                             {{5}, leaf}})},
                                                {{},
                                                 mtkv(NODE, {{{4}, leaf}, //
                                                             {{5}, leaf}, //
                                                             {{6}, leaf}, //
                                                             {{7}, leaf}})}});
  auto t_partitioned_2_expected =
      mtkv(NODE, {{{0}, t_sub_partitioned_expected},
                  {{1}, t_sub_partitioned_expected}});

  BOOST_TEST(*t_partitioned_2 == *t_partitioned_2_expected);
}
BOOST_DATA_TEST_CASE(test_qho_dimension_0_norest_existing_halo,
                     bdata::xrange(2), _bc) {
  // In the existing halo case BC should not make a difference
  TreeFactory f;
  auto leaf = mtkv(LEAF, {});
  auto t = f.generate_nd_tree({6});
  // First partitioning done with periodic boundary conditions.
  auto t_partitioned_1 = f.qh(t, // tree
                              0, // level
                              2, // nparts
                              1, // halo
                              0, // existing halo
                              hypercubes::slow::BoundaryCondition::OPEN); // bc
  auto bc2 = (hypercubes::slow::BoundaryCondition)_bc;
  auto t_partitioned_2 = f.qh(t_partitioned_1, // tree
                              1,               // level
                              3,               // nparts
                              1,               // halo
                              1,               // existing halo
                              bc2);            // bc

  auto t_partitioned_2_expected =
      mtkv(NODE, {{{0},
                   mtkv(NODE, {{{},
                                mtkv(NODE, {{{0}, leaf}, //
                                            {{1}, leaf}, //
                                            {{2}, leaf}})},
                               {{},
                                mtkv(NODE, {{{1}, leaf}, //
                                            {{2}, leaf}, //
                                            {{3}, leaf}})},
                               {{},
                                mtkv(NODE, {{{2}, leaf}, //
                                            {{3}, leaf}, //
                                            {{4}, leaf}})}})},
                  {{1},
                   mtkv(NODE, {{{},
                                mtkv(NODE, {{{0}, leaf}, //
                                            {{1}, leaf}, //
                                            {{2}, leaf}})},
                               {{},
                                mtkv(NODE, {{{1}, leaf}, //
                                            {{2}, leaf}, //
                                            {{3}, leaf}})},
                               {{},
                                mtkv(NODE, {{{2}, leaf}, //
                                            {{3}, leaf}, //
                                            {{4}, leaf}})}})}});

  BOOST_TEST(*t_partitioned_2 == *t_partitioned_2_expected);
}
// TODO: test that when there is an existing halo,
//       periodic boundary conditions should not be given?

BOOST_AUTO_TEST_CASE(test_bb_dimension_0) {
  TreeFactory f;
  auto leaf = mtkv(LEAF, {});
  auto t = f.generate_nd_tree({7});
  auto t_partitioned = f.bb(t, 0, 2);
  auto t_partitioned_expected = mtkv(NODE, {{{},
                                             mtkv(NODE, {{{0}, leaf},    //
                                                         {{1}, leaf}})}, //
                                            {{},
                                             mtkv(NODE, {{{2}, leaf},    //
                                                         {{3}, leaf},    //
                                                         {{4}, leaf}})}, //
                                            {{},
                                             mtkv(NODE, {{{5}, leaf}, //
                                                         {{6}, leaf}})}});

  BOOST_TEST(*t_partitioned == *t_partitioned_expected);
}
BOOST_AUTO_TEST_CASE(test_bb_dimension_1) {
  TreeFactory f;
  auto leaf = mtkv(LEAF, {});
  auto t = f.generate_nd_tree({3, 7});
  auto t_partitioned = f.bb(t, 1, 2);
  auto subtree = mtkv(NODE, {{{},
                              mtkv(NODE, {{{0}, leaf}, //
                                          {{1}, leaf}})},
                             //
                             {{},
                              mtkv(NODE, {{{2}, leaf}, //
                                          {{3}, leaf}, //
                                          {{4}, leaf}})},
                             //
                             {{},
                              mtkv(NODE, {{{5}, leaf}, //
                                          {{6}, leaf}})}});

  auto t_partitioned_expected = mtkv(NODE, {{{0}, subtree}, //
                                            {{1}, subtree}, //
                                            {{2}, subtree}});
  BOOST_TEST(*t_partitioned == *t_partitioned_expected);
}
BOOST_AUTO_TEST_CASE(test_bb_no_empty_trees) {
  TreeFactory f;
  auto leaf = mtkv(LEAF, {});
  auto t = f.generate_nd_tree({2});
  auto t_partitioned = f.bb(t, 0, 1);
  auto t_partitioned_expected = mtkv(
      NODE, {{{}, mtkv(NODE, {{{0}, leaf}})}, {{}, mtkv(NODE, {{{1}, leaf}})}});

  BOOST_TEST(*t_partitioned == *t_partitioned_expected);
}

BOOST_AUTO_TEST_CASE(test_hbb_dimension_0) {
  TreeFactory f;
  auto leaf = mtkv(LEAF, {});
  auto t = f.generate_nd_tree({11});
  auto t_partitioned = f.hbb(t, 0, 2);
  auto t_partitioned_expected = mtkv(NODE, {{{},
                                             mtkv(NODE, {{{0}, leaf}, //
                                                         {{1}, leaf}})},
                                            {{},
                                             mtkv(NODE, {{{2}, leaf},    //
                                                         {{3}, leaf}})}, //
                                            {{},
                                             mtkv(NODE, {{{4}, leaf},    //
                                                         {{5}, leaf},    //
                                                         {{6}, leaf}})}, //
                                            {{},
                                             mtkv(NODE, {{{7}, leaf}, //
                                                         {{8}, leaf}})},
                                            {{},
                                             mtkv(NODE, {{{9}, leaf}, //
                                                         {{10}, leaf}})}});

  BOOST_TEST(*t_partitioned == *t_partitioned_expected);
}
BOOST_AUTO_TEST_CASE(test_hbb_dimension_1) {
  TreeFactory f;
  auto leaf = mtkv(LEAF, {});
  auto t = f.generate_nd_tree({3, 11});
  auto t_partitioned = f.hbb(t, 1, 2);
  auto subtree = mtkv(NODE, {{{},
                              mtkv(NODE, {{{0}, leaf}, //
                                          {{1}, leaf}})},
                             {{},
                              mtkv(NODE, {{{2}, leaf},    //
                                          {{3}, leaf}})}, //
                             {{},
                              mtkv(NODE, {{{4}, leaf},    //
                                          {{5}, leaf},    //
                                          {{6}, leaf}})}, //
                             {{},
                              mtkv(NODE, {{{7}, leaf}, //
                                          {{8}, leaf}})},
                             {{},
                              mtkv(NODE, {{{9}, leaf}, //
                                          {{10}, leaf}})}});

  auto t_partitioned_expected = mtkv(NODE, {{{0}, subtree}, //
                                            {{1}, subtree}, //
                                            {{2}, subtree}});
  BOOST_TEST(*t_partitioned == *t_partitioned_expected);
}
BOOST_AUTO_TEST_CASE(test_hbb_throws_size_too_small) {
  TreeFactory f;
  auto leaf = mtkv(LEAF, {});
  auto t = f.generate_nd_tree({4});
  BOOST_CHECK_THROW(f.hbb(t, 0, 1), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_flatten_empty_children) {
  TreeFactory f;
  auto leaf = mtkv(LEAF, {});
  auto nkc = TreeFactory::no_key;
  auto t = mtkv(NODE, {{{},
                        mtkv(NODE, {{{nkc}, leaf}, //
                                    {{0}, leaf},   //
                                    {{1}, leaf},   //
                                    {{2}, leaf}})},
                       {{},
                        mtkv(NODE, {{{1}, leaf},    //
                                    {{2}, leaf},    //
                                    {{3}, leaf},    //
                                    {{4}, leaf}})}, //
                       {{},
                        mtkv(NODE, {{{3}, leaf}, //
                                    {{4}, leaf}, //
                                    {{5}, leaf}, //
                                    {{nkc}, leaf}})}});
  auto t_collapsed = f.flatten(t, 0, 2);

  auto t_collapsed_expected = mtkv(NODE, {{{0, 0}, leaf}, //
                                          {{0, 1}, leaf}, //
                                          {{0, 2}, leaf}, //
                                          {{0, 3}, leaf}, //
                                          {{1, 0}, leaf}, //
                                          {{1, 1}, leaf}, //
                                          {{1, 2}, leaf}, //
                                          {{1, 3}, leaf}, //
                                          {{2, 0}, leaf}, //
                                          {{2, 1}, leaf}, //
                                          {{2, 2}, leaf}, //
                                          {{2, 3}, leaf}});
  BOOST_TEST(*t_collapsed == *t_collapsed_expected);
}
BOOST_AUTO_TEST_CASE(test_flatten_empty_children_with_sub) {
  TreeFactory f;
  auto leaf = mtkv(LEAF, {});
  auto nkc = TreeFactory::no_key;
  auto subtree = mtkv(NODE, {{{0}, leaf}, //
                             {{1}, leaf}});
  auto t = mtkv(NODE, {{{},
                        mtkv(NODE, {{{nkc}, leaf}, //
                                    {{2}, subtree}})},
                       {{},
                        mtkv(NODE, {{{1}, subtree},    //
                                    {{4}, subtree}})}, //
                       {{},
                        mtkv(NODE, {{{3}, subtree}, //
                                    {{nkc}, leaf}})}});
  auto t_collapsed = f.flatten(t, 0, 3);

  // no {0,0} and no {2,1}
  auto t_collapsed_expected = mtkv(NODE, {{{0, 1, 0}, leaf}, //
                                          {{0, 1, 1}, leaf}, //
                                          {{1, 0, 0}, leaf}, //
                                          {{1, 0, 1}, leaf}, //
                                          {{1, 1, 0}, leaf}, //
                                          {{1, 1, 1}, leaf}, //
                                          {{2, 0, 0}, leaf}, //
                                          {{2, 0, 1}, leaf}});
  BOOST_TEST(*t_collapsed == *t_collapsed_expected);
}
BOOST_AUTO_TEST_CASE(test_flatten_levels_2d) {
  TreeFactory f;
  auto leaf = mtkv(LEAF, {});
  auto t = f.generate_nd_tree({2, 2});
  auto t_collapsed = f.flatten(t, 0, 2);
  auto t_collapsed_expected = mtkv(NODE, {{{0, 0}, leaf}, //
                                          {{0, 1}, leaf}, //
                                          {{1, 0}, leaf}, //
                                          {{1, 1}, leaf}});
  BOOST_TEST(*t_collapsed == *t_collapsed_expected);
}
BOOST_AUTO_TEST_CASE(test_flatten_levels_3d_13) {
  TreeFactory f;
  auto leaf = mtkv(LEAF, {});
  auto t = f.generate_nd_tree({2, 2, 2});
  auto t_collapsed = f.flatten(t, 1, 3);
  auto subtree = mtkv(NODE, {{{0, 0}, leaf}, //
                             {{0, 1}, leaf}, //
                             {{1, 0}, leaf}, //
                             {{1, 1}, leaf}});
  auto t_collapsed_expected = mtkv(NODE, {{{0}, subtree}, {{1}, subtree}});
  BOOST_TEST(*t_collapsed == *t_collapsed_expected);
}
BOOST_AUTO_TEST_CASE(test_flatten_levels_3d_02) {
  TreeFactory f;
  auto t = f.generate_nd_tree({2, 2, 2});
  auto t_collapsed = f.flatten(t, 0, 2);

  auto subtree = f.generate_nd_tree({2});
  auto t_collapsed_expected = mtkv(NODE, {{{0, 0}, subtree}, //
                                          {{0, 1}, subtree}, //
                                          {{1, 0}, subtree}, //
                                          {{1, 1}, subtree}});
  BOOST_TEST(*t_collapsed == *t_collapsed_expected);
}

BOOST_AUTO_TEST_CASE(test_eo_naive_1d_even) {
  TreeFactory f;
  auto leaf = mtkv(LEAF, {});
  auto t = f.generate_nd_tree({4});
  auto t_nested = f.qh(t, 0, 1, 0, 0, BoundaryCondition::OPEN);
  auto t_sub_expected = mtkv(NODE, {{{},
                                     mtkv(NODE, {{{0}, leaf},    //
                                                 {{2}, leaf}})}, //
                                    {{},
                                     mtkv(NODE, {{{1}, leaf}, //
                                                 {{3}, leaf}})}});

  auto t_partitioned = f.eo_naive(t_nested, 1);
  auto t_expected = mtkv(NODE, {{{0}, t_sub_expected}});
  BOOST_TEST(*t_partitioned == *t_expected);
}
BOOST_AUTO_TEST_CASE(test_eo_naive_1d_odd) {
  TreeFactory f;
  auto leaf = mtkv(LEAF, {});
  auto t = f.generate_nd_tree({5});
  auto t_nested = f.qh(t, 0, 1, 0, 0, BoundaryCondition::OPEN);
  auto t_sub_expected = mtkv(NODE, {{{},
                                     mtkv(NODE, {{{0}, leaf},    //
                                                 {{2}, leaf},    //
                                                 {{4}, leaf}})}, //
                                    {{},
                                     mtkv(NODE, {{{1}, leaf}, //
                                                 {{3}, leaf}})}});

  auto t_partitioned = f.eo_naive(t_nested, 1);
  auto t_expected = mtkv(NODE, {{{0}, t_sub_expected}});
  BOOST_TEST(*t_partitioned == *t_expected);
}
BOOST_AUTO_TEST_CASE(test_eo_naive_no_empty_tree) {
  TreeFactory f;
  auto t = f.generate_nd_tree({1});
  auto t_partitioned = f.eo_naive(t, 0);
  auto leaf = mtkv(LEAF, {});
  auto t_partitioned_expected = mtkv(NODE, {{{}, mtkv(NODE, {{{0}, leaf}})}});
  BOOST_TEST(*t_partitioned == *t_partitioned_expected);
}

BOOST_AUTO_TEST_CASE(test_remap_level) {
  TreeFactory f;
  auto leaf = mtkv(LEAF, {});
  auto subtree = mtkv(NODE, {{{10}, mtkv(NODE, {})},            //
                             {{11}, mtkv(NODE, {{{6}, leaf}})}, //
                             {{12}, mtkv(NODE, {{{8}, leaf}})}, //
                             {{13}, mtkv(NODE, {})}});          //
  auto t0 = mtkv(NODE, {{{0}, subtree},                         //
                        {{1}, subtree}});
  vector<int> remapping{3, 0, 2, 1};
  auto t1 = f.remap_level(t0, 1, remapping);
  auto subtree1_exp = mtkv(NODE, {{{3}, mtkv(NODE, {})},              //
                                  {{0}, mtkv(NODE, {})},              //
                                  {{2}, mtkv(NODE, {{{0}, leaf}})},   //
                                  {{1}, mtkv(NODE, {{{0}, leaf}})}}); //
  auto t1_exp = mtkv(NODE, {{{0}, subtree1_exp},                      //
                            {{1}, subtree1_exp}});
  BOOST_TEST(*t1 == *t1_exp);
}

BOOST_AUTO_TEST_CASE(test_collect_leaves_constructor) {
  TreeFactory f;
  auto t = f.generate_nd_tree({2, 2});
  auto t_collapsed = f.collect_leaves(t, 0, 4);

  auto leaf = mtkv(LEAF, {});

  auto t_collapsed_expected = mtkv(NODE, {{{0, 0}, leaf}, //
                                          {{0, 1}, leaf}, //
                                          {{1, 0}, leaf}, //
                                          {{1, 1}, leaf}});
  BOOST_TEST(*t_collapsed == *t_collapsed_expected);
}
BOOST_AUTO_TEST_CASE(test_collect_leaves_constructor_padding) {
  TreeFactory f;
  auto t = f.generate_nd_tree({2, 3});
  auto t_collapsed = f.collect_leaves(t, 0, 8);

  auto leaf = mtkv(LEAF, {});

  auto nkc = TreeFactory::no_key;

  auto t_collapsed_expected = mtkv(NODE, {{{0, 0}, leaf}, //
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
BOOST_AUTO_TEST_CASE(test_collect_leaves_constructor_padding_midlevel) {
  TreeFactory f;
  auto t = f.generate_nd_tree({2, 2, 3});
  auto t_collapsed = f.collect_leaves(t, 1, 8);

  auto leaf = mtkv(LEAF, {});

  auto nkc = TreeFactory::no_key;

  auto sub_t_collapsed_expected = mtkv(NODE, {{{0, 0}, leaf}, //
                                              {{0, 1}, leaf}, //
                                              {{0, 2}, leaf}, //
                                              {{1, 0}, leaf}, //
                                              {{1, 1}, leaf}, //
                                              {{1, 2}, leaf}, //
                                              // these do not exist
                                              // in the original tree
                                              {{nkc, nkc}, leaf}, //
                                              {{nkc, nkc}, leaf}});
  auto t_collapsed_expected = mtkv(NODE, {{{0}, sub_t_collapsed_expected}, //
                                          {{1}, sub_t_collapsed_expected}});
  BOOST_TEST(*t_collapsed == *t_collapsed_expected);
}
// index pullback and pushforward
BOOST_AUTO_TEST_CASE(test_index_pullback_id) {
  TreeFactory f;
  auto t = f.generate_nd_tree({2, 2, 2});
  vector<int> in{0, 1, 0};
  auto out = index_pullback(t, in);
  BOOST_TEST(in == out);
}
BOOST_AUTO_TEST_CASE(test_index_pullback_complex) {
  auto leaf = mtkv(LEAF, {});
  auto subtree = mtkv(NODE, {{{10}, mtkv(NODE, {})},            //
                             {{11}, mtkv(NODE, {{{6}, leaf}})}, //
                             {{12}, mtkv(NODE, {{{8}, leaf}})}, //
                             {{13}, mtkv(NODE, {})}});          //
  auto t0 = mtkv(NODE, {{{40}, subtree},                        //
                        {{21}, subtree}});

  vector<int> in{0, 2, 0};
  vector<int> out_expected{40, 12, 8};
  auto out = index_pullback(t0, in);
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_expected.begin(), out_expected.end());
}
BOOST_AUTO_TEST_CASE(test_index_pullback_throws) {
  TreeFactory f;
  auto t = f.generate_nd_tree({2, 4, 4});
  BOOST_CHECK_THROW(index_pullback(t, {1, 4, 3}), KeyNotFoundError);
}
BOOST_AUTO_TEST_CASE(test_index_pullback_throws_negative) {
  TreeFactory f;
  auto t = f.generate_nd_tree({2, 4, 4});
  BOOST_CHECK_THROW(index_pullback(t, {1, 1, -1}), KeyNotFoundError);
}

BOOST_AUTO_TEST_CASE(test_index_pushforward_id) {
  TreeFactory f;
  auto t = f.generate_nd_tree({2, 2, 2});
  vector<int> in{0, 1, 0};
  auto out = index_pushforward(t, in);
  BOOST_TEST(vector<vector<int>>{in} == out);
}
BOOST_AUTO_TEST_CASE(test_index_pushforward_complex) {
  auto leaf = mtkv(LEAF, {});
  auto subtree = mtkv(NODE, {{{10}, mtkv(NODE, {})},            //
                             {{11}, mtkv(NODE, {{{6}, leaf}})}, //
                             {{12}, mtkv(NODE, {{{8}, leaf}})}, //
                             {{13}, mtkv(NODE, {})}});          //
  auto t0 = mtkv(NODE, {{{40}, subtree},                        //
                        {{21}, subtree}});

  vector<int> in{40, 12, 8};
  vector<int> out_expected{0, 2, 0};
  auto out = index_pushforward(t0, in)[0];
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_expected.begin(), out_expected.end());
}

BOOST_AUTO_TEST_CASE(test_index_pullback_and_pushforward) {
  /// Pushforward is the opposite of pullback.
  auto leaf = mtkv(LEAF, {});
  auto subtree = mtkv(NODE, {{{10}, mtkv(NODE, {})},            //
                             {{11}, mtkv(NODE, {{{6}, leaf}})}, //
                             {{12}, mtkv(NODE, {{{8}, leaf}})}, //
                             {{13}, mtkv(NODE, {})}});          //
  auto t0 = mtkv(NODE, {{{40}, subtree},                        //
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
BOOST_AUTO_TEST_CASE(test_index_pushforward_noresults) {
  TreeFactory f;
  auto t = f.generate_nd_tree({2, 4, 4});
  auto res = index_pushforward(t, {1, 4, 3});
  BOOST_TEST(res.size() == 0);
}

BOOST_AUTO_TEST_CASE(test_index_pullback_qh_nohalo) {
  TreeFactory f;
  auto t0 = f.generate_nd_tree({2, 4, 4});
  int level = 1;
  auto t1 = f.qh(t0, level, 2, 0, 0, BoundaryCondition::OPEN);
  vector<int> in{0, 1, 1, 2};
  vector<int> out_exp{0, 3, 2};
  auto out = index_pullback(t1, in);
  BOOST_TEST(out.size() == 3);
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}
BOOST_AUTO_TEST_CASE(test_index_pushforward_qh_nohalo) {
  TreeFactory f;
  auto t0 = f.generate_nd_tree({2, 4, 4});
  int level = 1;
  auto t1 = f.qh(t0, level, 2, 0, 0, BoundaryCondition::OPEN);
  vector<int> in{0, 3, 2};
  vector<int> out_exp{0, 1, 1, 2};
  auto out = index_pushforward(t1, in)[0];
  BOOST_TEST(out.size() == 4);
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}

BOOST_AUTO_TEST_CASE(test_index_pullback_qh_halo_p) {
  TreeFactory f;
  auto t = f.generate_nd_tree({6});
  auto t_partitioned = f.qh(t, // tree
                            0, // level
                            3, // nparts
                            1, // halo
                            0, // existing halo
                            hypercubes::slow::BoundaryCondition::PERIODIC);

  vector<int> in{1, 2};
  vector<int> out_exp{3};
  auto out = index_pullback(t_partitioned, in);
  BOOST_TEST(out.size() == 1);
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}
BOOST_AUTO_TEST_CASE(test_index_pushforward_qh_halo_p) {
  TreeFactory f;
  auto t = f.generate_nd_tree({6});
  auto t_partitioned = f.qh(t, // tree
                            0, // level
                            3, // nparts
                            1, // halo
                            0, // existing halo
                            hypercubes::slow::BoundaryCondition::PERIODIC);

  vector<int> in{3};
  vector<vector<int>> out_exp{{1, 2}, {2, 0}};
  auto out = index_pushforward(t_partitioned, in);
  BOOST_TEST(out.size() == 2);
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}
BOOST_AUTO_TEST_CASE(test_index_pullback_pad_qh_halo_o_null) {
  TreeFactory f;
  auto t = f.generate_nd_tree({6});
  auto t_partitioned = f.qh(t, // tree
                            0, // level
                            3, // nparts
                            1, // halo
                            0, // existing halo
                            hypercubes::slow::BoundaryCondition::OPEN);

  vector<int> in{0, 0};
  auto out = index_pullback_safe(t_partitioned, in);
  BOOST_TEST(out.size() == 0);
}
BOOST_AUTO_TEST_CASE(test_index_pullback_inverse_tree) {
  TreeFactory f;
  auto t0 = f.generate_nd_tree({2, 4, 4});
  int level = 1;
  auto t1 = f.qh(t0, level, 2, 0, 0, BoundaryCondition::OPEN);
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
  TreeFactory f;
  auto t0 = f.generate_nd_tree({2, 4, 4});
  int level = 1;
  auto t1 = f.qh(t0, level, 2, 0, 0, BoundaryCondition::OPEN);
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

BOOST_AUTO_TEST_CASE(test_remap_level_pullback_index) {
  TreeFactory f;
  auto leaf = mtkv(LEAF, {});
  auto subtree = mtkv(NODE, {{{10}, mtkv(NODE, {})},            //
                             {{11}, mtkv(NODE, {{{6}, leaf}})}, //
                             {{12}, mtkv(NODE, {{{8}, leaf}})}, //
                             {{13}, mtkv(NODE, {})}});          //
  auto t0 = mtkv(NODE, {{{0}, subtree},                         //
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
  TreeFactory f;
  auto leaf = mtkv(LEAF, {});
  auto subtree = mtkv(NODE, {{{10}, mtkv(NODE, {})},            //
                             {{11}, mtkv(NODE, {{{6}, leaf}})}, //
                             {{12}, mtkv(NODE, {{{8}, leaf}})}, //
                             {{13}, mtkv(NODE, {})}});          //
  auto t0 = mtkv(NODE, {{{0}, subtree},                         //
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
  TreeFactory f;
  auto t = f.generate_nd_tree({2, 2, 2});
  auto t_collapsed = f.flatten(t, 0, 2);

  auto subtree = f.generate_nd_tree({2});
  auto t_collapsed_expected = mtkv(NODE, {{{0, 0}, subtree}, //
                                          {{0, 1}, subtree}, //
                                          {{1, 0}, subtree}, //
                                          {{1, 1}, subtree}});
  auto out = index_pullback(t_collapsed, {2, 0});
  decltype(out) out_expected{1, 0, 0};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_expected.begin(), out_expected.end());
}
BOOST_AUTO_TEST_CASE(test_flatten_pushforward) {
  TreeFactory f;
  auto t = f.generate_nd_tree({2, 2, 2});
  auto t_collapsed = f.flatten(t, 0, 2);

  auto subtree = f.generate_nd_tree({2});
  auto t_collapsed_expected = mtkv(NODE, {{{0, 0}, subtree}, //
                                          {{0, 1}, subtree}, //
                                          {{1, 0}, subtree}, //
                                          {{1, 1}, subtree}});
  auto out = index_pushforward(t_collapsed, {1, 0, 0})[0];
  decltype(out) out_expected{2, 0};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_expected.begin(), out_expected.end());
}

BOOST_AUTO_TEST_CASE(test_collect_leaves_pullback_pad) {
  TreeFactory f;
  auto t = f.generate_nd_tree({2, 3});
  auto t_collapsed = f.collect_leaves(t, 0, 8);

  auto nkc = TreeFactory::no_key;
  auto out_wrong = index_pullback(t_collapsed, {7});
  vector<int> out_wrong_expected{nkc, nkc};
  BOOST_TEST(out_wrong == out_wrong_expected);
  auto out = index_pullback_safe(t_collapsed, {7});
  BOOST_TEST(out.size() == 0);
}
BOOST_AUTO_TEST_CASE(test_collect_leaves_pushforward) {
  TreeFactory f;
  auto t = f.generate_nd_tree({2, 3});
  auto t_collapsed = f.collect_leaves(t, 0, 8);

  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 3; ++j) {
      auto out = index_pushforward(t_collapsed, {i, j})[0];
      auto in = index_pullback_safe(t_collapsed, out)[0];
      BOOST_TEST(in == vector<int>({i, j}));
    }
}

// TODO: Test that instead of crashes meaningful exceptions are thrown
//       (is this a good thing?)
BOOST_AUTO_TEST_CASE(test_swap_levels_by_key) {
  TreeFactory f;

  auto t = mtkv(1, {{{10},
                     mtkv(2, {{{12},
                               mtkv(3, {{{14}, mtkv(5, {})},       //
                                        {{15}, mtkv(6, {})}})}})}, //
                    {{11},
                     mtkv(4, {{{13},
                               mtkv(3, {{{14}, mtkv(7, {})}, //
                                        {{15}, mtkv(8, {})}})}})}});
  vector<int> new_level_ordering{2, 0, 1};

  auto newt_exp = mtkv(
      n(3), {{{14},
              mtkv(n(1), {{{10}, mtkv(n(2), {{{12}, mtkv(n(5), {})}})},    //
                          {{11}, mtkv(n(4), {{{13}, mtkv(n(7), {})}})}})}, //
             {{15},
              mtkv(n(1), {{{10}, mtkv(n(2), {{{12}, mtkv(n(6), {})}})}, //
                          {{11}, mtkv(n(4), {{{13}, mtkv(n(8), {})}})}})}});

  auto newt = f.swap_levels(convert_tree(t), new_level_ordering);

  BOOST_TEST(*newt == *newt_exp);
}
BOOST_AUTO_TEST_CASE(test_renumber_children) {
  TreeFactory f;

  auto t =
      mtkv(n(1), {{{10},
                   mtkv(n(2), {{{12},
                                mtkv(n(3), {{{14}, mtkv(n(5), {})},       //
                                            {{15}, mtkv(n(6), {})}})}})}, //
                  {{11},
                   mtkv(n(4), {{{13},
                                mtkv(n(3), {{{14}, mtkv(n(7), {})}, //
                                            {{15}, mtkv(n(8), {})}})}})}});

  auto expren =
      mtkv(n(1), {{{0},
                   mtkv(n(2), {{{0},
                                mtkv(n(3), {{{0}, mtkv(n(5), {})},       //
                                            {{1}, mtkv(n(6), {})}})}})}, //
                  {{1},
                   mtkv(n(4), {{{0},
                                mtkv(n(3), {{{0}, mtkv(n(7), {})}, //
                                            {{1}, mtkv(n(8), {})}})}})}});
  auto ren = f.renumber_children(t);
  BOOST_TEST(*ren == *expren);
}

BOOST_AUTO_TEST_CASE(test_bring_level_on_top_key) {
  TreeFactory f;
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
  auto transf_expected = convert_tree(pull_keys_up(_transf_expected));

  auto transf = f.bring_level_on_top(convert_tree(t), 2);

  BOOST_TEST(*transf_expected == *transf);
}

BOOST_AUTO_TEST_CASE(test_bring_level_on_top_key_throws) {
  TreeFactory f;
  auto _t =
      mt(mp(0, 1),
         {mt(mp(20, 2), {mt(mp(22, 3 /*different*/), {mt(mp(24, 4), {}), //
                                                      mt(mp(25, 5), {})})}),
          mt(mp(21, 9), {mt(mp(23, 4 /*different*/), {mt(mp(24, 11), {}), //
                                                      mt(mp(26, 12), {})})})});

  auto t = pull_keys_up(_t);
  BOOST_CHECK_THROW(f.bring_level_on_top(convert_tree(t), 2),
                    TreeLevelPermutationError);
}

BOOST_AUTO_TEST_CASE(test_collapse_level_by_key_all_there) {
  TreeFactory f;
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
  auto tcollapsed_exp = convert_tree(pull_keys_up(_tcollapsed_exp));

  auto tcollapsed = f.collapse_level(convert_tree(t),         //
                                     1 /*level to collapse*/, //
                                     {5} /*child key to replace*/);

  BOOST_TEST(*tcollapsed_exp == *tcollapsed);
}
BOOST_AUTO_TEST_CASE(test_collapse_level_by_key_missing) {
  TreeFactory f;
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
  auto tcollapsed_exp = convert_tree(pull_keys_up(_tcollapsed_exp));

  auto tcollapsed = f.collapse_level(convert_tree(t),         //
                                     1 /*level to collapse*/, //
                                     {5} /*child key to replace*/);

  BOOST_TEST(*tcollapsed_exp == *tcollapsed);
}

BOOST_AUTO_TEST_CASE(test_selector_true) {
  TreeFactory f;
  TreeFactory::Predicate p = [](const std::vector<int> &idx) {
    return BoolM::T;
  };
  auto t = mtkv(NodeType::NODE, {{{10}, mtkv(NodeType::LEAF, {})}, //
                                 {{11}, mtkv(NodeType::LEAF, {})}, //
                                 {{22}, mtkv(NodeType::LEAF, {})}});

  // Trees are renumbered.
  auto exp_newt = mtkv(NodeType::NODE, {{{0}, mtkv(NodeType::LEAF, {})}, //
                                        {{1}, mtkv(NodeType::LEAF, {})}, //
                                        {{2}, mtkv(NodeType::LEAF, {})}});

  auto newt = f.select_subtree(t, p);

  BOOST_TEST(*newt == *exp_newt);
}
BOOST_AUTO_TEST_CASE(test_selector_false) {
  TreeFactory f;
  TreeFactory::Predicate p = [](const std::vector<int> &idx) {
    if (idx.size() < 2)
      return BoolM::M;
    else if (idx[1] == 1)
      return BoolM::T;
    else
      return BoolM::F;
  };
  auto subt = mtkv(NodeType::NODE, {{{10}, mtkv(NodeType::LEAF, {})}, //
                                    {{11}, mtkv(NodeType::LEAF, {})}, //
                                    {{22}, mtkv(NodeType::LEAF, {})}});

  auto t = mtkv(NodeType::NODE, {{{10}, subt}, //
                                 {{32}, subt}});

  auto newt = f.select_subtree(t, p);

  auto exp_newt =
      mtkv(NodeType::NODE,
           {{{0}, mtkv(NodeType::NODE, {{{1}, mtkv(NodeType::LEAF, {})}})},
            {{1}, mtkv(NodeType::NODE, {{{1}, mtkv(NodeType::LEAF, {})}})}});

  BOOST_TEST(*newt == *exp_newt);
}
BOOST_AUTO_TEST_CASE(test_selector_prune_empty_subtrees) {
  TreeFactory f;
  TreeFactory::Predicate p = [](const std::vector<int> &idx) {
    if (idx.size() < 2)
      return BoolM::M;
    else if (idx[1] == 2)
      return BoolM::T;
    else
      return BoolM::F;
  };
  auto subt0 = mtkv(NodeType::NODE, {{{10}, mtkv(NodeType::LEAF, {})}, //
                                     {{11}, mtkv(NodeType::LEAF, {})}, //
                                     {{22}, mtkv(NodeType::LEAF, {})}});

  auto subt1 = mtkv(NodeType::NODE, {{{10}, mtkv(NodeType::LEAF, {})}, //
                                     {{22}, mtkv(NodeType::LEAF, {})}});

  auto t = mtkv(NodeType::NODE, {{{10}, subt0}, //
                                 {{32}, subt1}});

  auto newt = f.select_subtree(t, p);

  auto exp_newt =
      mtkv(NodeType::NODE,
           {{{0}, mtkv(NodeType::NODE, {{{2}, mtkv(NodeType::LEAF, {})}})}});

  BOOST_TEST(*newt == *exp_newt);
}
BOOST_AUTO_TEST_CASE(test_selector_prune_empty_subtrees_leave_leaves) {
  TreeFactory f;
  TreeFactory::Predicate p = [](const std::vector<int> &idx) {
    return BoolM::M;
  };
  auto subt = mtkv(NodeType::NODE, {{{0}, mtkv(NodeType::LEAF, {})}, //
                                    {{1}, mtkv(NodeType::LEAF, {})}, //
                                    {{2}, mtkv(NodeType::LEAF, {})}});

  auto t = mtkv(NodeType::NODE, {{{0}, subt}, //
                                 {{1}, subt}});

  auto newt = f.select_subtree(t, p);

  BOOST_TEST(*newt == *t);
}

BOOST_AUTO_TEST_CASE(test_eo_fix) {

  TreeFactory f;
  // A 9x9 lattice partitioned in 3x3xEOxflattened index,
  // with 3 local degrees of freedom
  auto local = mtkv(NodeType::NODE, {{{0}, mtkv(NodeType::LEAF, {})},
                                     {{1}, mtkv(NodeType::LEAF, {})},
                                     {{2}, mtkv(NodeType::LEAF, {})}});

  auto sub4 = mtkv(NodeType::NODE, {{{0}, local}, //
                                    {{1}, local}, //
                                    {{2}, local}, //
                                    {{3}, local}});
  auto sub5 = mtkv(NodeType::NODE, {{{0}, local}, //
                                    {{1}, local}, //
                                    {{2}, local}, //
                                    {{3}, local}, //
                                    {{4}, local}});

  auto sub54 = mtkv(NodeType::NODE, {{{0}, sub5}, //
                                     {{1}, sub4}});

  auto sub45 = mtkv(NodeType::NODE, {{{0}, sub4}, //
                                     {{1}, sub5}});

  auto row = mtkv(NodeType::NODE, {{{0}, sub54}, //
                                   {{1}, sub54}, //
                                   {{2}, sub54}});

  auto t = mtkv(NodeType::NODE, {{{0}, row}, //
                                 {{1}, row}, //
                                 {{2}, row}});

  auto transform = [](const vector<int> idx) -> vector<vector<int>> {
    int r = idx[0];
    int c = idx[1];
    int parity = idx[2];
    int flattened = idx[3];
    int local = idx[4];
    int global_row = r * 3 + (flattened * 2 + parity) / 3;
    int global_col = c * 3 + (flattened * 2 + parity) % 3;
    vector<vector<int>> res{{global_row, global_col, local}};

    return res;
  };

  auto new_t = f.eo_fix(t,         //
                        2,         // level,
                        transform, //
                        {0, 1});

  auto row13_fix = mtkv(NodeType::NODE, {{{0}, sub54}, //
                                         {{1}, sub45}, //
                                         {{2}, sub54}});

  auto row2_fix = mtkv(NodeType::NODE, {{{0}, sub45}, //
                                        {{1}, sub54}, //
                                        {{2}, sub45}});

  auto t_fix = mtkv(NodeType::NODE, {{{0}, row13_fix}, //
                                     {{1}, row2_fix},  //
                                     {{2}, row13_fix}});
  BOOST_TEST(*t_fix == *new_t);
}

BOOST_AUTO_TEST_SUITE_END()
