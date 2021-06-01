#include "fixtures.hpp"
#include "trees/level_swap.hpp"
#include "trees/tree.hpp"
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

using namespace hypercubes::slow;
using namespace hypercubes::slow::internals;

BOOST_AUTO_TEST_SUITE(test_level_swap)

BOOST_AUTO_TEST_CASE(test_list_permutation) {

  std::vector<int> in{4, 3, 5, 2, 1, 0, 8, 7, 6};
  std::vector<int> out{0, 1, 2, 3, 4, 5, 6, 7, 8};

  auto res = find_permutation(in, out);
  BOOST_TEST(res == in);
}

BOOST_AUTO_TEST_CASE(test_list_permutation_different_lenghts) {

  std::vector<int> in{4, 3, 5, 2, 1, 0, 8, 7, 6};
  std::vector<int> out{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  BOOST_CHECK_THROW(find_permutation(in, out), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_list_permutation_missing_case) {

  std::vector<int> in{4, 3, 5, 2, 1, 0, 8, 7, 6};
  std::vector<int> out{0, 1, 2, 3, 4, 5, 6, 7, 9};

  BOOST_CHECK_THROW(find_permutation(in, out), std::invalid_argument);
}

BOOST_FIXTURE_TEST_CASE(test_get_partitioners_names, Part1D42) {
  auto names = get_partitioners_names(partitioners);
  std::vector<std::string> exp_names{
      "MPI X",     //
      "Vector X",  //
      "Halo X",    //
      "EO",        //
      "Remainder", //
      "Site",      //
  };
  BOOST_TEST(exp_names == names);
}

// This is already an integration test.
BOOST_FIXTURE_TEST_CASE(test_swap_levels_max_idx_tree, Part1D42) {
  auto max_idx_tree = get_max_idx_tree(t);
  std::vector<std::string> reordered_names{
      "MPI X",     //
      "EO",        //
      "Halo X",    //
      "Vector X",  //
      "Remainder", //
      "Site",      //
  };
  auto new_level_ordering =
      find_permutation(get_partitioners_names(partitioners), reordered_names);
  std::vector<int> exp_new_level_ordering{0, 3, 2, 1, 4, 5};
  BOOST_TEST(new_level_ordering == exp_new_level_ordering); //

  // Just to have these functions compiled
  // so that they can be used in gdb.
  int a = get_max_depth(max_idx_tree); // FOR DEBUG
  auto st = tree_str(*max_idx_tree);   // FOR DEBUG

  auto max_idx_tree_swapped = swap_levels(max_idx_tree, new_level_ordering);
  auto ml = [](int i) { return mt(i, {}); };
  decltype(max_idx_tree_swapped) exptree_nosites =
      // MPI    EO     HBB    Vec    Remainder   // MPI Vec Hbb EO Start
      mt(4, {mt(2, {mt(5, {mt(2, {ml(0),         // 0   0   0   0  -1
                                  ml(0)}),       // 0   1   0   0  5
                           mt(2, {ml(1),         // 0   0   1   0  0
                                  ml(1)}),       // 0   1   1   0  6
                           mt(2, {ml(2),         // 0   0   2   0  1
                                  ml(1)}),       // 0   1   2   0  7
                           mt(2, {ml(0),         // 0   0   3   0  5
                                  ml(1)}),       // 0   1   3   0  10
                           mt(2, {ml(1),         // 0   0   4   0  6
                                  ml(0)})}),     // 0   1   4   0  11
                    mt(5, {mt(2, {ml(1),         // 0   0   0   1  -1
                                  ml(1)}),       // 0   1   0   1  5
                           mt(2, {ml(0),         // 0   0   1   1  0
                                  ml(0)}),       // 0   1   1   1  6
                           mt(2, {ml(2),         // 0   0   2   1  1
                                  ml(2)}),       // 0   1   2   1  7
                           mt(2, {ml(1),         // 0   0   3   1  5
                                  ml(0)}),       // 0   1   3   1  10
                           mt(2, {ml(0),         // 0   0   4   1  6
                                  ml(1)})})}),   // 0   1   4   1  11
             mt(2, {mt(5, {mt(2, {ml(1),         // 1   0   0   0  10
                                  ml(1)}),       // 1   1   0   0  16
                           mt(2, {ml(0),         // 1   0   1   0  11
                                  ml(0)}),       // 1   1   1   0  17
                           mt(2, {ml(2),         // 1   0   2   0  12
                                  ml(2)}),       // 1   1   2   0  18
                           mt(2, {ml(1),         // 1   0   3   0  16
                                  ml(0)}),       // 1   1   3   0  21
                           mt(2, {ml(0),         // 1   0   4   0  17
                                  ml(1)})}),     // 1   1   4   0  22
                    mt(5, {mt(2, {ml(0),         // 1   0   0   1  10
                                  ml(0)}),       // 1   1   0   1  16
                           mt(2, {ml(1),         // 1   0   1   1  11
                                  ml(1)}),       // 1   1   1   1  17
                           mt(2, {ml(2),         // 1   0   2   1  12
                                  ml(1)}),       // 1   1   2   1  18
                           mt(2, {ml(0),         // 1   0   3   1  16
                                  ml(1)}),       // 1   1   3   1  21
                           mt(2, {ml(1),         // 1   0   4   1  17
                                  ml(0)})})}),   // 1   1   4   1  22
             mt(2, {mt(5, {mt(2, {ml(0),         // 2   0   0   0  21
                                  ml(0)}),       // 2   1   0   0  27
                           mt(2, {ml(1),         // 2   0   1   0  22
                                  ml(1)}),       // 2   1   1   0  28
                           mt(2, {ml(2),         // 2   0   2   0  23
                                  ml(1)}),       // 2   1   2   0  29
                           mt(2, {ml(0),         // 2   0   3   0  27
                                  ml(1)}),       // 2   1   3   0  32
                           mt(2, {ml(1),         // 2   0   4   0  28
                                  ml(0)})}),     // 2   1   4   0  33
                    mt(5, {mt(2, {ml(1),         // 2   0   0   1  21
                                  ml(1)}),       // 2   1   0   1  27
                           mt(2, {ml(0),         // 2   0   1   1  22
                                  ml(0)}),       // 2   1   1   1  28
                           mt(2, {ml(2),         // 2   0   2   1  23
                                  ml(2)}),       // 2   1   2   1  29
                           mt(2, {ml(1),         // 2   0   3   1  27
                                  ml(0)}),       // 2   1   3   1  32
                           mt(2, {ml(0),         // 2   0   4   1  28
                                  ml(1)})})}),   // 2   1   4   1  33
             mt(2, {mt(5, {mt(2, {ml(1),         // 3   0   0   0  32
                                  ml(0)}),       // 3   1   0   0  37
                           mt(2, {ml(0),         // 3   0   1   0  33
                                  ml(1)}),       // 3   1   1   0  38
                           mt(2, {ml(2),         // 3   0   2   0  34
                                  ml(1)}),       // 3   1   2   0  39
                           mt(2, {ml(0),         // 3   0   3   0  37
                                  ml(0)}),       // 3   1   3   0  41
                           mt(2, {ml(1),         // 3   0   4   0  38
                                  ml(1)})}),     // 3   1   4   0  42
                    mt(5, {mt(2, {ml(0),         // 3   0   0   1  32
                                  ml(1)}),       // 3   1   0   1  37
                           mt(2, {ml(1),         // 3   0   1   1  33
                                  ml(0)}),       // 3   1   1   1  38
                           mt(2, {ml(1),         // 3   0   2   1  34
                                  ml(1)}),       // 3   1   2   1  39
                           mt(2, {ml(1),         // 3   0   3   1  37
                                  ml(1)}),       // 3   1   3   1  41
                           mt(2, {ml(0),         // 3   0   4   1  38
                                  ml(0)})})})}); // 3   1   4   1  42
  BOOST_TEST(*truncate_tree(max_idx_tree_swapped,
                            get_max_depth(max_idx_tree_swapped) - 1) ==
             *exptree_nosites);
}

BOOST_AUTO_TEST_SUITE_END()
