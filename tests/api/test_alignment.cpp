#include "api/memory_layout.hpp"
#include "geometry/geometry.hpp"
#include "selectors/partition_predicates.hpp"
#include <boost/test/unit_test.hpp>

using namespace hypercubes::slow;
namespace pm = hypercubes::slow::partitioner_makers;

BOOST_AUTO_TEST_SUITE(test_alignment)
BOOST_AUTO_TEST_CASE(test_alignment_grid2D) {
  enum { X, Y, MATROW, EXTRA };
  PartList partitioners{pm::QPeriodic("MPI X", X, 4),      // 0
                        pm::QPeriodic("MPI Y", Y, 4),      // 1
                        pm::QOpen("Vector X", X, 2),       // 2
                        pm::QOpen("Vector Y", Y, 2),       // 3
                        pm::HBB("Halo X", X, 1),           // 4
                        pm::HBB("Halo Y", Y, 1),           // 5
                        pm::EO("EO", {true, true, false}), // 6
                        pm::Plain("Local-matrow", MATROW), // 7
                        pm::Plain("Extra", EXTRA),         // 8
                        pm::Site()};                       // 9
  auto partition_tree = PartitionTree({48, 48, 3}, partitioners, {MATROW});
  const int virtual_node_level = 4;

  auto size_tree =
      partition_tree
          .nchildren_tree()                                       //
          .prune(getp(selectors::mpi_rank, partitioners, {0, 0})) //
          .permute({"MPI X",                                      //
                    "MPI Y",                                      //
                    "EO",                                         //
                    "Local-matrow",                               //
                    "Halo X",                                     //
                    "Halo Y",                                     //
                    "Extra",                                      //
                    "Vector X",                                   //
                    "Vector Y",                                   //
                    "Site"})                                      //
          .size_tree();                                           //
  auto offset_tree = size_tree.offset_tree();                     //

  auto no_halos = getp(selectors::halos_upto_NmD, partitioners, 0);
  int total_size = size_tree.get_size({});

  vector<BoundaryCondition> bcs{BoundaryCondition::PERIODIC, //
                                BoundaryCondition::PERIODIC};
  Sizes sizes{48, 48};

  int vector_length = 4;
  BOOST_TEST(total_size % vector_length == 0);

  auto po_matcher = get_level_matcher(partition_tree, offset_tree);
  auto op_matcher = get_level_matcher(offset_tree, partition_tree);

  using nnf = std::function<Coordinates(Coordinates)>;
  int caseidx = 0;
  for (auto fneigh :
       {nnf([&](Coordinates xs) { return up(xs, sizes, bcs, X); }),   //
        nnf([&](Coordinates xs) { return down(xs, sizes, bcs, X); }), //
        nnf([&](Coordinates xs) { return up(xs, sizes, bcs, Y); }),   //
        nnf([&](Coordinates xs) { return down(xs, sizes, bcs, Y); })}) {

    for (int offset = 0;      //
         offset < total_size; //
         offset += vector_length) {

      vector<int> neighbours_offsets(vector_length);
      Indices idxs_base = offset_tree.get_indices(offset);
      if (no_halos(idxs_base) != BoolM::T)
        continue;
      for (int lane = 0; lane < vector_length; ++lane) {
        Indices idxs = offset_tree.get_indices(offset + lane);
        Indices pt_idxs =
            op_matcher(idxs); // TODO: Consider alternatives. This is risky.
        Coordinates coords = partition_tree.get_coordinates(pt_idxs);
        Coordinates neigh_coords = fneigh(coords);
        auto all_neigh_idxs = partition_tree.get_indices_wg(neigh_coords);
        Indices pt_neigh_idxs =
            std::find_if(all_neigh_idxs.begin(), //
                         all_neigh_idxs.end(),   //
                         [&](auto f_idxs) {
                           Indices idxs_candidate = f_idxs.second;
                           // accept the candidate
                           // that is in the same virtual node
                           // as the original site
                           for (int level = 0;              //
                                level < virtual_node_level; //
                                ++level)
                             if (idxs_candidate[level] != pt_idxs[level])
                               return false;
                           return true;
                         })
                ->second;
        Indices neigh_idxs = po_matcher(
            pt_neigh_idxs); // TODO: Consider alternatives. This is risky.
        neighbours_offsets[lane] = offset_tree.get_offset(neigh_idxs);

        BOOST_TEST(neighbours_offsets[lane] - neighbours_offsets[0] == lane);
      }
    }
    caseidx++;
  }
}

BOOST_AUTO_TEST_SUITE_END()
