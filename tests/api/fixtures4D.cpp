#include "fixtures4D.hpp"
#include "api/memory_layout.hpp"
#include <iostream>
namespace hypercubes {
namespace slow {

namespace pm = hypercubes::slow::partitioner_makers;
GridLikeBase::GridLikeBase()
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
GridLikeNChildren::GridLikeNChildren()
    : GridLikeBase(), //
      predicate(getp(selectors::mpi_rank, partitioners, {2, 3, 1, 1})),
      skeleton_tree(SkeletonTree(partition_tree).prune(predicate)){};

GridLikeSize::GridLikeSize() : GridLikeNChildren(), size_tree(skeleton_tree){};
GridLikeOffset::GridLikeOffset() : GridLikeSize(), offset_tree(size_tree){};
} // namespace slow
} // namespace hypercubes
