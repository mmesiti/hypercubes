#include "fixtures2D.hpp"
#include <boost/test/unit_test.hpp>

using namespace hypercubes::slow;

BOOST_AUTO_TEST_SUITE(test_offset_tree)

BOOST_FIXTURE_TEST_CASE(match_permutation, GridLike2DOffset) {
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
  auto permuted = nchildren_tree.permute(permuted_level_names);

  auto matcher = get_level_matcher(nchildren_tree, permuted);
  auto swapped_level_names = matcher(nchildren_tree.get_level_names());

  BOOST_TEST(swapped_level_names == permuted.get_level_names());
}

BOOST_AUTO_TEST_SUITE_END()
