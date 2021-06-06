#include <boost/test/unit_test.hpp>

#include "api/memory_layout.hpp"
#include "geometry/geometry.hpp"
#include "partitioners/partitioners.hpp"
#include "trees/partition_tree.hpp"

using namespace hypercubes::slow;
namespace pm = hypercubes::slow::partitioner_makers;

struct GridLike {

  enum { X, Y, Z, T, MATROW, MATCOL, EXTRA };
  vector<int> nonspatial_dimensions{MATROW, MATCOL};
  Sizes sizes;
  PartList partitioners;
  PartitionTree partition_tree;

  GridLike()
      : sizes({42, 42, 42, 42, 3, 3}),
        partitioners({
            pm::QPeriodic("MPI X", X, 4),                         //
            pm::QPeriodic("MPI Y", Y, 4),                         //
            pm::QPeriodic("MPI Z", Z, 4),                         //
            pm::QPeriodic("MPI T", T, 4),                         //
            pm::QOpen("Vector X", X, 2),                          //
            pm::QOpen("Vector Y", Y, 2),                          //
            pm::HBB("Halo X", X, 1),                              //
            pm::HBB("Halo Y", Y, 1),                              //
            pm::HBB("Halo Z", Z, 1),                              //
            pm::HBB("Halo T", T, 1),                              //
            pm::EO("EO", {true, true, true, true, false, false}), //
            pm::Plain("Local-matrow", MATROW),                    //
            pm::Plain("Local-matcol", MATCOL),                    //
            pm::Plain("Extra", EXTRA),                            //
            pm::Site()                                            //
        }),
        partition_tree(sizes, partitioners, nonspatial_dimensions) {}
};

BOOST_AUTO_TEST_SUITE(test_grid_like)

BOOST_FIXTURE_TEST_CASE(test_grid_like_get_indices, GridLike) {
  Sizes sizes{42, 42, 42, 42, 3, 3};
  SizeParityD spd = add_parity(sizes, {MATROW, MATCOL});
  internals::PartitionTree pt =
      internals::get_partition_treeM(spd, partitioners);
  Coordinates xs{3, 23, 39, 15, 1, 2};
  Indices idxs = internals::get_real_indices(pt, xs);
  BOOST_TEST(idxs == partition_tree.get_indices(xs));
}

BOOST_FIXTURE_TEST_CASE(test_grid_like_get_indices_wg, GridLike) {
  Sizes sizes{42, 42, 42, 42, 3, 3};
  SizeParityD spd = add_parity(sizes, {MATROW, MATCOL});
  internals::PartitionTree pt =
      internals::get_partition_treeM(spd, partitioners);
  Coordinates xs{3, 23, 39, 15, 1, 2};
  auto tree_indices = internals::get_indices_tree_with_ghosts(pt, xs);
  auto idxs = internals::get_relevant_indices_flat(tree_indices);

  BOOST_TEST(idxs == partition_tree.get_indices_wg(xs));
}

BOOST_AUTO_TEST_SUITE_END()
