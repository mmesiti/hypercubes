#include <boost/test/unit_test.hpp>

#include "api/memory_layout.hpp"
#include "geometry/geometry.hpp"
#include "partitioners/partitioners.hpp"
#include "selectors/partition_predicates.hpp"
#include "trees/partition_tree.hpp"

using namespace hypercubes::slow;
namespace pm = hypercubes::slow::partitioner_makers;

struct GridLikeBase {

  enum { X, Y, Z, T, MATROW, MATCOL, EXTRA };
  vector<int> nonspatial_dimensions{MATROW, MATCOL};
  Sizes sizes;
  PartList partitioners;
  PartitionTree partition_tree;

  GridLikeBase()
      : sizes({48, 48, 42, 42, 3, 3}),
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

struct GridLikeNChildren : public GridLikeBase {
  NChildrenTree nchildren_tree;
  GridLikeNChildren()
      : GridLikeBase(),                                     //
        nchildren_tree(partition_tree,                      //
                       get_mpi_rank_predicate(partitioners, //
                                              {2, 3, 1, 1})

        ){};
};

BOOST_AUTO_TEST_SUITE(test_grid_like)
/*
**
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
BOOST_FIXTURE_TEST_CASE(test_nchildren_tree_get_subtree, GridLikeNChildren) {

  NChildrenTree subtree = nchildren_tree.get_subtree(Indices{
      2, 3, 1, 1, // MPI
                  // coordinates: 24,36,11,11
      0, 1,       // Vector
                  // coordinates: 24,42,11,11
      1, 1, 1, 1, // HBB
                  // coordinates: 24,42,11,11
  });
  // site is EVEN
  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };
  using internals::mt;
  auto expected = mt(mp(1, 2), // last HHB T index
                     {
                         mt(mp(0, 3),                          // site is even
                            {mt(mp(0, 3), {mt(mp(0, 1), {}),   //
                                           mt(mp(1, 1), {}),   //
                                           mt(mp(2, 1), {})}), //
                             mt(mp(1, 3), {mt(mp(0, 1), {}),   //
                                           mt(mp(1, 1), {}),   //
                                           mt(mp(2, 1), {})}), //
                             mt(mp(2, 3), {mt(mp(0, 1), {}),   //
                                           mt(mp(1, 1), {}),   //
                                           mt(mp(2, 1), {})})}),
                         mt(mp(1, 3),          // odd does not exist
                            {mt(mp(0, 3), {}), //
                             mt(mp(1, 3), {}), //
                             mt(mp(2, 3), {})}),

                     });
  auto truncated = internals::truncate_tree(subtree.get_internal(), 4);
  BOOST_TEST(*(truncated) == *expected);
}
 */

BOOST_FIXTURE_TEST_CASE(test_nchildren_permute, GridLikeNChildren) {
  vector<std::string> permuted_level_names{
      "MPI X",        //
      "MPI Y",        //
      "MPI Z",        //
      "MPI T",        //
      "EO",           //
      "Local-matrow", //
      "Local-matcol", //
      "Halo X",       //
      "Halo Y",       //
      "Halo Z",       //
      "Halo T",       //
      "Extra",        //
      "Vector X",     //
      "Vector Y",     //
      "Site",
  };
  NChildrenTree permuted_tree = nchildren_tree.permute(permuted_level_names);
}

BOOST_AUTO_TEST_SUITE_END()
