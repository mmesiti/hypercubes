#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <iostream>

#include "fixtures1D.hpp"
#include "fixtures2D.hpp"
#include "fixtures4D.hpp"

using namespace hypercubes::slow;

BOOST_AUTO_TEST_SUITE(test_offset_tree)

// Offset
BOOST_FIXTURE_TEST_CASE(test_offset_dot, GridLike1DOffset) {
  auto offset_tree_2 = size_tree.offset_tree();
  BOOST_TEST(*offset_tree.get_internal() == *offset_tree_2.get_internal());
}
BOOST_FIXTURE_TEST_CASE(test_offset_tree_get_subtree, GridLikeOffset) {

  OffsetTree subtree_eo = offset_tree.get_subtree(Indices{
      2, 3, 1, 1, // MPI
                  // coordinates: 24,36,11,11
      0, 1,       // Vector
                  // coordinates: 24,42,11,11
      1, 1, 1, 1, // HBB
                  // coordinates: 24,42,11,11
  });
  // site is EVEN
  OffsetTree subtree_e = subtree_eo.get_subtree(Indices{0});
  int main_offset = subtree_e.get_offset({});
  int mo = main_offset;
  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };
  using internals::mt;
  auto exp_e =
      mt(mp(0, mo),                                          // site is even
         {mt(mp(0, mo), {mt(mp(0, mo), {mt(mp(0, mo), {})}), //
                         mt(mp(1, mo + 1), {mt(mp(0, mo + 1), {})}),       //
                         mt(mp(2, mo + 2), {mt(mp(0, mo + 2), {})})}),     //
          mt(mp(1, mo + 3), {mt(mp(0, mo + 3), {mt(mp(0, mo + 3), {})}),   //
                             mt(mp(1, mo + 4), {mt(mp(0, mo + 4), {})}),   //
                             mt(mp(2, mo + 5), {mt(mp(0, mo + 5), {})})}), //
          mt(mp(2, mo + 6), {mt(mp(0, mo + 6), {mt(mp(0, mo + 6), {})}),   //
                             mt(mp(1, mo + 7), {mt(mp(0, mo + 7), {})}),   //
                             mt(mp(2, mo + 8), {mt(mp(0, mo + 8), {})})})});

  BOOST_TEST(*(subtree_e.get_internal()) == *exp_e);
  // there are no odd sites
  BOOST_CHECK_THROW(subtree_eo.get_subtree(Indices{1}), std::invalid_argument);
}
BOOST_FIXTURE_TEST_CASE(test_offset_shift_subtree, GridLikeOffset) {

  OffsetTree subtree_eo = offset_tree.get_subtree(Indices{
      2, 3, 1, 1, // MPI
                  // coordinates: 24,36,11,11
      0, 1,       // Vector
                  // coordinates: 24,42,11,11
      1, 1, 1, 1, // HBB
                  // coordinates: 24,42,11,11
  });
  // site is EVEN
  OffsetTree subtree_e = subtree_eo.get_subtree(Indices{0});
  int main_offset = subtree_e.get_offset({});
  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };
  using internals::mt;
  auto exp_e = mt(mp(0, 0), // site is even
                  {mt(mp(0, 0), {mt(mp(0, 0), {mt(mp(0, 0), {})}),   //
                                 mt(mp(1, 1), {mt(mp(0, 1), {})}),   //
                                 mt(mp(2, 2), {mt(mp(0, 2), {})})}), //
                   mt(mp(1, 3), {mt(mp(0, 3), {mt(mp(0, 3), {})}),   //
                                 mt(mp(1, 4), {mt(mp(0, 4), {})}),   //
                                 mt(mp(2, 5), {mt(mp(0, 5), {})})}), //
                   mt(mp(2, 6), {mt(mp(0, 6), {mt(mp(0, 6), {})}),   //
                                 mt(mp(1, 7), {mt(mp(0, 7), {})}),   //
                                 mt(mp(2, 8), {mt(mp(0, 8), {})})})});

  auto subtree_shifted = subtree_e.shift(-main_offset);
  BOOST_TEST(*(subtree_shifted.get_internal()) == *exp_e);
}
BOOST_FIXTURE_TEST_CASE(test_offset_get_indices_roundtrip, GridLike2DOffset) {

  Indices i{2, 1, 0, 1, 2, 2, 0, 2, 0};
  int offset = offset_tree.get_offset(i);
  Indices i_roundtrip = offset_tree.get_indices(offset);
  BOOST_TEST(i == i_roundtrip);
}
BOOST_FIXTURE_TEST_CASE(test_offset_get_indices_roundtrip_many,
                        GridLike2DOffset) {
  for (int offset = 0; offset < 2 *     // Vec X
                                    2 * // Vec Y
                                    8 * // X hbb ( 1+6+1 )
                                    8 * // Y hbb ( 1+6+1 )
                                    3;  // local dof
       ++offset) {
    Indices i = offset_tree.get_indices(offset);
    int offset_roundtrip = offset_tree.get_offset(i);
    BOOST_TEST(offset == offset_roundtrip);
  };
}
BOOST_FIXTURE_TEST_CASE(test_get_subtree_level_names, GridLike1DOffset) {
  BOOST_TEST(offset_tree.get_subtree({2, 0, 2}).get_level_names() ==
             vector<std::string>({"EO", "Local-matrow", "Extra", "Site"}));
}
BOOST_AUTO_TEST_SUITE_END()
