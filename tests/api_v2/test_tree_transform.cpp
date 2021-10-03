#include "api_v2/tree_transform.hpp"
#include "trees/tree.hpp"
#include "trees/tree_data_structure.hpp"
#include <boost/test/unit_test.hpp>

using namespace hypercubes::slow::internals;

BOOST_AUTO_TEST_SUITE(test_tree_transform)

BOOST_AUTO_TEST_CASE(test_generate_nd_tree) {

  auto t = generate_nd_tree({2, 3});
  auto expected = mt(false, {mt(false, {mt(true, {}),     //
                                        mt(true, {}),     //
                                        mt(true, {})}),   //
                             mt(false, {mt(true, {}),     //
                                        mt(true, {}),     //
                                        mt(true, {})})}); //

  BOOST_TEST(*t == *expected);
}

BOOST_AUTO_TEST_CASE(test_partition_q_dimension_0_no_rest) {
  auto t = generate_nd_tree({15});
  auto t_partitioned = q(t, 0, 3);
  auto t_partitioned_expected = generate_nd_tree({3, 5});
  BOOST_TEST(*t_partitioned == *t_partitioned_expected);
}

BOOST_AUTO_TEST_CASE(test_partition_q_dimension_0_with_rest) {
  auto t = generate_nd_tree({5});
  auto t_partitioned = q(t, 0, 2);
  auto t_partitioned_expected = mt(false, {mt(false, {mt(true, {}),   //
                                                      mt(true, {}),   //
                                                      mt(true, {})}), //
                                           mt(false, {mt(true, {}),   //
                                                      mt(true, {})})});
  BOOST_TEST(*t_partitioned == *t_partitioned_expected);
}

BOOST_AUTO_TEST_CASE(test_partition_q_dimension_1_with_rest) {
  auto t = generate_nd_tree({2, 5});
  auto t_partitioned = q(t, 1, 2);

  auto subtree = mt(false, {mt(false, {mt(true, {}),   //
                                       mt(true, {}),   //
                                       mt(true, {})}), //
                            mt(false, {mt(true, {}),   //
                                       mt(true, {})})});
  auto t_partitioned_expected = mt(false, {subtree, subtree});
  BOOST_TEST(*t_partitioned == *t_partitioned_expected);
}

BOOST_AUTO_TEST_CASE(test_partition_q_dimension_0_as_equal_as_possible) {
  auto t = generate_nd_tree({23});
  auto t_partitioned = q(t, 0, 19);
  BOOST_TEST(t_partitioned->children.size() == 19);
  for (auto c : t_partitioned->children) {
    bool success = c->children.size() == 1 or c->children.size() == 2;
    BOOST_TEST(success);
  }
}

BOOST_AUTO_TEST_CASE(test_bb_dimension_0) {
  auto t = generate_nd_tree({7});
  auto t_partitioned = bb(t, 0, 2);
  auto t_partitioned_expected = mt(false, {mt(false, {mt(true, {}),   //
                                                      mt(true, {})}), //
                                           mt(false, {mt(true, {}),   //
                                                      mt(true, {}),   //
                                                      mt(true, {})}), //
                                           mt(false, {mt(true, {}),   //
                                                      mt(true, {})})});
  BOOST_TEST(*t_partitioned == *t_partitioned_expected);
}
BOOST_AUTO_TEST_CASE(test_bb_dimension_1) {
  auto t = generate_nd_tree({3, 7});
  auto t_partitioned = bb(t, 1, 2);
  auto subtree = mt(false, {mt(false, {mt(true, {}),   //
                                       mt(true, {})}), //
                            mt(false, {mt(true, {}),   //
                                       mt(true, {}),   //
                                       mt(true, {})}), //
                            mt(false, {mt(true, {}),   //
                                       mt(true, {})})});
  auto t_partitioned_expected = mt(false, {subtree, subtree, subtree});
  BOOST_TEST(*t_partitioned == *t_partitioned_expected);
}
BOOST_AUTO_TEST_SUITE_END()
