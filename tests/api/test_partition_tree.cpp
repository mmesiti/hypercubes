#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <iostream>

#include "fixtures1D.hpp"
#include "fixtures2D.hpp"
#include "fixtures4D.hpp"

using namespace hypercubes::slow;

BOOST_AUTO_TEST_SUITE(test_partition_tree)
// PartitionTree
BOOST_FIXTURE_TEST_CASE(test_grid_like_get_indices, GridLikeBase) {
  Sizes sizes{48, 48, 42, 42, 3, 3};
  SizeParityD spd = add_parity(sizes, {MATROW, MATCOL});
  internals::PartitionTree pt =
      internals::get_partition_treeM(spd, partitioners);
  Coordinates xs{3, 23, 39, 15, 1, 2};
  Indices idxs = internals::get_real_indices(pt, xs);
  BOOST_TEST(idxs == partition_tree.get_indices(xs));
}
BOOST_FIXTURE_TEST_CASE(test_grid_like_get_indices_wg, GridLikeBase) {
  Sizes sizes{48, 48, 42, 42, 3, 3};
  SizeParityD spd = add_parity(sizes, {MATROW, MATCOL});
  internals::PartitionTree pt =
      internals::get_partition_treeM(spd, partitioners);
  Coordinates xs{3, 23, 39, 15, 1, 2};
  auto tree_indices = internals::get_indices_tree_with_ghosts(pt, xs);
  auto idxs = internals::get_relevant_indices_flat(tree_indices);

  BOOST_TEST(idxs == partition_tree.get_indices_wg(xs));
}
BOOST_FIXTURE_TEST_CASE(test_grid_like_get_coordinates, GridLikeBase) {
  Coordinates xs{3, 23, 39, 15, 1, 2};
  Indices idxs = partition_tree.get_indices(xs);

  BOOST_TEST(xs == partition_tree.get_coordinates(idxs));
}
BOOST_FIXTURE_TEST_CASE(test_get_partitioners_name, GridLikeBase1D) {

  BOOST_TEST(partition_tree.get_level_names() ==
             vector<std::string>({"MPI X", "Vector X", "Halo X", "EO",
                                  "Local-matrow", "Extra", "Site"}));
}

BOOST_AUTO_TEST_SUITE_END()
