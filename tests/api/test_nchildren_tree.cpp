#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <iostream>

#include "fixtures1D.hpp"
#include "fixtures2D.hpp"
#include "fixtures4D.hpp"

using namespace hypercubes::slow;

BOOST_AUTO_TEST_SUITE(test_nchildren_tree)
BOOST_FIXTURE_TEST_CASE(test_nchildren_tree_get_subtree, GridLikeNChildren) {

  SkeletonTree subtree_eo = skeleton_tree.get_subtree(Indices{
      2, 3, 1, 1, // MPI
                  // coordinates: 24,36,11,11
      0, 1,       // Vector
                  // coordinates: 24,42,11,11
      1, 1, 1, 1, // HBB
                  // coordinates: 24,42,11,11
  });
  // site is EVEN
  SkeletonTree subtree_e = subtree_eo.get_subtree(Indices{0});
  SkeletonTree subtree_o = subtree_eo.get_subtree(Indices{1});
  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };
  using internals::mt;
  auto exp_e = mt(mp(0, 0), // site is even
                  {mt(mp(0, 0), {mt(mp(0, 0), {mt(mp(0, 1), {})}),   //
                                 mt(mp(1, 0), {mt(mp(0, 1), {})}),   //
                                 mt(mp(2, 0), {mt(mp(0, 1), {})})}), //
                   mt(mp(1, 0), {mt(mp(0, 0), {mt(mp(0, 1), {})}),   //
                                 mt(mp(1, 0), {mt(mp(0, 1), {})}),   //
                                 mt(mp(2, 0), {mt(mp(0, 1), {})})}), //
                   mt(mp(2, 0), {mt(mp(0, 0), {mt(mp(0, 1), {})}),   //
                                 mt(mp(1, 0), {mt(mp(0, 1), {})}),   //
                                 mt(mp(2, 0), {mt(mp(0, 1), {})})})});

  // there are no sites
  auto exp_o = mt(mp(1, 0),                          // no odd sites
                  {mt(mp(0, 0), {mt(mp(0, 0), {}),   //
                                 mt(mp(1, 0), {}),   //
                                 mt(mp(2, 0), {})}), //
                   mt(mp(1, 0), {mt(mp(0, 0), {}),   //
                                 mt(mp(1, 0), {}),   //
                                 mt(mp(2, 0), {})}), //
                   mt(mp(2, 0), {mt(mp(0, 0), {}),   //
                                 mt(mp(1, 0), {}),   //
                                 mt(mp(2, 0), {})})});

  BOOST_TEST(*(subtree_e.get_internal()) == *exp_e);
  BOOST_TEST(*(subtree_o.get_internal()) == *exp_o);
}
BOOST_FIXTURE_TEST_CASE(test_nchildren_dot, GridLike1DNChildren) {

  auto nchildren_tree_2 = partition_tree.skeleton_tree().prune(predicate);
  BOOST_TEST(*nchildren_tree_2.get_internal() == *skeleton_tree.get_internal());
}
BOOST_FIXTURE_TEST_CASE(test_get_nchildren, GridLike1DNChildren) {
  BOOST_TEST(skeleton_tree.get_nchildren(Indices{2, 0}) == 5);
}
BOOST_FIXTURE_TEST_CASE(test_nchildren_permute2D, GridLike2DNChildren) {
  vector<std::string> permuted_level_names{
      "MPI X",        // 0
      "MPI Y",        // 1
      "EO",           // 2
      "Local-matrow", // 3
      "Halo X",       // 4
      "Halo Y",       // 5
      "Extra",        // 6
      "Vector X",     // 7
      "Vector Y",     // 8
      "Site",         // 9
  };
  SkeletonTree permuted_tree = skeleton_tree.permute(permuted_level_names);
  // To level 4 - Halo X
  BOOST_TEST(permuted_tree.get_nchildren(Indices{2, 1, 0, 0}) == 5);
  // To level 7 - Vector X
  BOOST_TEST(permuted_tree.get_nchildren(Indices{2, 1, 0, 0, 2, 2, 0}) == 2);
  // To level 8 - Vector Y
  BOOST_TEST(permuted_tree.get_nchildren(Indices{2, 1, 0, 0, 2, 2, 0, 0}) == 2);
  // To level 9 - Site
  BOOST_TEST(permuted_tree.get_nchildren(Indices{2, 1, 0, 0, 2, 2, 0, 0, 1}) ==
             0);
  // Invalid - out of pruned
  BOOST_CHECK_THROW(permuted_tree.get_nchildren(Indices{2, 2, 0, 0}),
                    std::invalid_argument);
}
BOOST_FIXTURE_TEST_CASE(test_nchildren_permute, GridLikeNChildren) {
  vector<std::string> permuted_level_names{
      "MPI X",        //  0
      "MPI Y",        //  1
      "MPI Z",        //  3
      "MPI T",        //  4
      "EO",           //  5
      "Local-matrow", //  6
      "Local-matcol", //  7
      "Halo X",       //  8
      "Halo Y",       //  9
      "Halo Z",       // 10
      "Halo T",       // 11
      "Extra",        // 12
      "Vector X",     // 13
      "Vector Y",     // 14
      "Site",         // 15
  };
  SkeletonTree permuted_tree = skeleton_tree.permute(permuted_level_names);
  // 4 - EO
  BOOST_TEST(permuted_tree.get_nchildren(Indices{2, 3, 1, 1}) == 2);
  // 5 - Local-matrow
  BOOST_TEST(permuted_tree.get_nchildren(Indices{2, 3, 1, 1, 0}) == 3);
  // 6 - Local-matcol
  BOOST_TEST(permuted_tree.get_nchildren(Indices{2, 3, 1, 1, 1, 2}) == 3);
  // 7 - HBB X
  BOOST_TEST(permuted_tree.get_nchildren(Indices{2, 3, 1, 1, 1, 2, 2}) == 5);
  // 12 - extra: sites are all even:
  // (24+5,36+5,11+10,11+10) = (29,41,21,21)
  // ... (3 other sites of the same parity)
  BOOST_TEST(permuted_tree.get_nchildren(Indices{2, 3, 1, 1, 1, 2, 2, //
                                                 3, 3, 3, 3}) == 0);
  // 12 - extra: sites are all odd:
  // (24,36+5,11,11) = (24,41,21,21)
  // ... (3 other sites of the same parity)
  BOOST_TEST(permuted_tree.get_nchildren(Indices{2, 3, 1, 1, 1, 2, 2, //
                                                 1, 3, 1, 1}) == 1);
  // 12 - extra: sites are half even and half odd:
  // 4*4*9*9 total sites / 2
  BOOST_TEST(permuted_tree.get_nchildren(Indices{2, 3, 1, 1, 1, 2, 2, //
                                                 2, 2, 2, 2}) ==
             4 * 4 * 9 * 9 / 2);
  // 12 - extra:
  // number of sites is odd
  // majority is even because the parity of the center is even.
  // Center:
  // (24,36,11+5,11+5) = (24,41,16,16)
  // 9*9 total sites / 2
  // ODD:
  BOOST_TEST(permuted_tree.get_nchildren(Indices{2, 3, 1, 1, 1, 2, 2, //
                                                 1, 1, 2, 2}) == 9 * 9 / 2);
  // EVEN:
  BOOST_TEST(permuted_tree.get_nchildren(Indices{2, 3, 1, 1, 0, 2, 2, //
                                                 1, 1, 2, 2}) == 9 * 9 / 2 + 1);
}
BOOST_FIXTURE_TEST_CASE(test_get_partitioners_name, GridLike1DNChildren) {

  BOOST_TEST(skeleton_tree.get_level_names() ==
             vector<std::string>({"MPI X", "Vector X", "Halo X", "EO",
                                  "Local-matrow", "Extra", "Site"}));
}
BOOST_FIXTURE_TEST_CASE(test_get_subtree_names, GridLike1DNChildren) {
  BOOST_TEST(skeleton_tree.get_subtree({2, 0, 2}).get_level_names() ==
             vector<std::string>({"EO", "Local-matrow", "Extra", "Site"}));
}

BOOST_AUTO_TEST_SUITE_END()
