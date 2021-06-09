#include <boost/test/unit_test.hpp>
#include <iostream>

#include "api/memory_layout.hpp"
#include "geometry/geometry.hpp"
#include "partitioners/partitioners.hpp"
#include "selectors/partition_predicates.hpp"
#include "trees/partition_tree.hpp"
#include "trees/tree.hpp"

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
            pm::QPeriodic("MPI X", X, 4),                         //  0
            pm::QPeriodic("MPI Y", Y, 4),                         //  1
            pm::QPeriodic("MPI Z", Z, 4),                         //  2
            pm::QPeriodic("MPI T", T, 4),                         //  3
            pm::QOpen("Vector X", X, 2),                          //  4
            pm::QOpen("Vector Y", Y, 2),                          //  5
            pm::HBB("Halo X", X, 1),                              //  6
            pm::HBB("Halo Y", Y, 1),                              //  7
            pm::HBB("Halo Z", Z, 1),                              //  8
            pm::HBB("Halo T", T, 1),                              //  9
            pm::EO("EO", {true, true, true, true, false, false}), // 10
            pm::Plain("Local-matrow", MATROW),                    // 11
            pm::Plain("Local-matcol", MATCOL),                    // 12
            pm::Plain("Extra", EXTRA),                            // 13
            pm::Site()                                            // 14
        }),
        partition_tree(sizes, partitioners, nonspatial_dimensions) {
    std::cout << "Max and min depths: ";
    std::cout << internals::get_max_depth(partition_tree.get_internal());
    std::cout << " ";
    std::cout << internals::get_min_depth(partition_tree.get_internal());
    std::cout << "\n";
  }
};

struct GridLikeNChildren : public GridLikeBase {
  NChildrenTree nchildren_tree;
  GridLikeNChildren()
      : GridLikeBase(),                                     //
        nchildren_tree(partition_tree,                      //
                       get_mpi_rank_predicate(partitioners, //
                                              {2, 3, 1, 1})){};
};

struct GridLikeBaseSmall {

  enum { X, MATROW, EXTRA };
  vector<int> nonspatial_dimensions{MATROW};
  Sizes sizes;
  PartList partitioners;
  PartitionTree partition_tree;

  GridLikeBaseSmall()
      : sizes({48, 3}), partitioners({
                            pm::QPeriodic("MPI X", X, 4),      // 0
                            pm::QOpen("Vector X", X, 2),       // 1
                            pm::HBB("Halo X", X, 1),           // 2
                            pm::EO("EO", {true, false}),       // 3
                            pm::Plain("Local-matrow", MATROW), // 4
                            pm::Plain("Extra", EXTRA),         // 5
                            pm::Site()                         // 6
                        }),
        partition_tree(sizes, partitioners, nonspatial_dimensions) {}
};
struct GridLikeSmallNChildren : public GridLikeBaseSmall {
  NChildrenTree nchildren_tree;
  GridLikeSmallNChildren()
      : GridLikeBaseSmall(),                                //
        nchildren_tree(partition_tree,                      //
                       get_mpi_rank_predicate(partitioners, //
                                              {2})

        ){};
};

BOOST_AUTO_TEST_SUITE(test_grid_like)
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

  NChildrenTree subtree_eo = nchildren_tree.get_subtree(Indices{
      2, 3, 1, 1, // MPI
                  // coordinates: 24,36,11,11
      0, 1,       // Vector
                  // coordinates: 24,42,11,11
      1, 1, 1, 1, // HBB
                  // coordinates: 24,42,11,11
  });
  // site is EVEN
  NChildrenTree subtree_e = subtree_eo.get_subtree(Indices{0});
  NChildrenTree subtree_o = subtree_eo.get_subtree(Indices{1});
  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };
  using internals::mt;
  auto exp_e = mt(mp(0, 3), // site is even
                  {mt(mp(0, 3), {mt(mp(0, 1), {mt(mp(0, 1), {})}),   //
                                 mt(mp(1, 1), {mt(mp(0, 1), {})}),   //
                                 mt(mp(2, 1), {mt(mp(0, 1), {})})}), //
                   mt(mp(1, 3), {mt(mp(0, 1), {mt(mp(0, 1), {})}),   //
                                 mt(mp(1, 1), {mt(mp(0, 1), {})}),   //
                                 mt(mp(2, 1), {mt(mp(0, 1), {})})}), //
                   mt(mp(2, 3), {mt(mp(0, 1), {mt(mp(0, 1), {})}),   //
                                 mt(mp(1, 1), {mt(mp(0, 1), {})}),   //
                                 mt(mp(2, 1), {mt(mp(0, 1), {})})})});

  // there are no sites
  auto exp_o = mt(mp(1, 3),                          // no odd sites
                  {mt(mp(0, 3), {mt(mp(0, 0), {}),   //
                                 mt(mp(1, 0), {}),   //
                                 mt(mp(2, 0), {})}), //
                   mt(mp(1, 3), {mt(mp(0, 0), {}),   //
                                 mt(mp(1, 0), {}),   //
                                 mt(mp(2, 0), {})}), //
                   mt(mp(2, 3), {mt(mp(0, 0), {}),   //
                                 mt(mp(1, 0), {}),   //
                                 mt(mp(2, 0), {})})});

  BOOST_TEST(*(subtree_e.get_internal()) == *exp_e);
  BOOST_TEST(*(subtree_o.get_internal()) == *exp_o);
}

BOOST_FIXTURE_TEST_CASE(test_nchildren_permute_small, GridLikeSmallNChildren) {
  vector<std::string> permuted_level_names{
      "MPI X",        // 0
      "EO",           // 1
      "Local-matrow", // 2
      "Halo X",       // 3
      "Extra",        // 4
      "Vector X",     // 5
      "Site",         // 6
  };
  NChildrenTree permuted_tree = nchildren_tree.permute(permuted_level_names);
  auto max_depth = internals::get_max_depth(partition_tree.get_internal());
  auto min_depth = internals::get_min_depth(partition_tree.get_internal());
}

BOOST_FIXTURE_TEST_CASE(test_nchildren_permute, GridLikeNChildren) {
  vector<std::string> permuted_level_names{
      "MPI X",        // 0
      "MPI Y",        // 0
      "MPI Z",        // 0
      "MPI T",        // 0
      "EO",           // 1
      "Local-matrow", // 2
      "Local-matcol", // 2
      "Halo X",       // 3
      "Halo Y",       // 3
      "Halo Z",       // 3
      "Halo T",       // 3
      "Extra",        // 4
      "Vector X",     // 5
      "Vector Y",     // 5
      "Site",         // 6
  };
  NChildrenTree permuted_tree = nchildren_tree.permute(permuted_level_names);
  auto max_depth = internals::get_max_depth(partition_tree.get_internal());
  auto min_depth = internals::get_min_depth(partition_tree.get_internal());
}
BOOST_AUTO_TEST_SUITE_END()
