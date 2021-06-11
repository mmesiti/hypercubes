#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <iostream>

#include "fixtures1D.hpp"
#include "fixtures2D.hpp"
#include "fixtures4D.hpp"

using namespace hypercubes::slow;

BOOST_AUTO_TEST_SUITE(test_size_tree)
// Size
BOOST_FIXTURE_TEST_CASE(test_size_dot, GridLike1DSize) {
  auto size_tree_2 = nchildren_tree.size_tree();
  BOOST_TEST(*size_tree_2.get_internal() == *size_tree.get_internal());
}
BOOST_FIXTURE_TEST_CASE(test_size_tree_get_subtree, GridLikeSize) {

  SizeTree subtree_eo = size_tree.get_subtree(Indices{
      2, 3, 1, 1, // MPI
                  // coordinates: 24,36,11,11
      0, 1,       // Vector
                  // coordinates: 24,42,11,11
      1, 1, 1, 1, // HBB
                  // coordinates: 24,42,11,11
  });
  // site is EVEN
  SizeTree subtree_e = subtree_eo.get_subtree(Indices{0});
  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };
  using internals::mt;
  auto exp_e = mt(mp(0, 9), // site is even
                  {mt(mp(0, 3), {mt(mp(0, 1), {mt(mp(0, 1), {})}),   //
                                 mt(mp(1, 1), {mt(mp(0, 1), {})}),   //
                                 mt(mp(2, 1), {mt(mp(0, 1), {})})}), //
                   mt(mp(1, 3), {mt(mp(0, 1), {mt(mp(0, 1), {})}),   //
                                 mt(mp(1, 1), {mt(mp(0, 1), {})}),   //
                                 mt(mp(2, 1), {mt(mp(0, 1), {})})}), //
                   mt(mp(2, 3), {mt(mp(0, 1), {mt(mp(0, 1), {})}),   //
                                 mt(mp(1, 1), {mt(mp(0, 1), {})}),   //
                                 mt(mp(2, 1), {mt(mp(0, 1), {})})})});

  BOOST_TEST(*(subtree_e.get_internal()) == *exp_e);
  // there are no odd sites
  BOOST_CHECK_THROW(subtree_eo.get_subtree(Indices{1}), std::invalid_argument);
}
BOOST_FIXTURE_TEST_CASE(test_get_size, GridLike2DSize) {
  BOOST_TEST(size_tree.get_size(Indices{2, 1}) //
             == 2 *                            // vec X
                    2 *                        // vec Y
                    8 *                        // 1+6+1 , hbb X
                    8 *                        // 1+6+1 , hbb 2
                    3);                        // local dof
}
BOOST_AUTO_TEST_SUITE_END()
