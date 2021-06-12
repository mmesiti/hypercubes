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
        partition_tree(sizes, partitioners, nonspatial_dimensions) {}
};

struct GridLikeNChildren : public GridLikeBase {
  NChildrenTree nchildren_tree;
  GridLikeNChildren()
      : GridLikeBase(), //
        nchildren_tree(NChildrenTree(partition_tree)
                           .prune(getp(mpi_rank, partitioners, //
                                       {0, 0, 0, 0}))){};
};

int main() { GridLikeNChildren test; }
