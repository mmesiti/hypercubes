#include "fixtures1D.hpp"

namespace hypercubes {
namespace slow {

namespace pm = hypercubes::slow::partitioner_makers;
GridLikeBase1D::GridLikeBase1D()

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

GridLike1DNChildren::GridLike1DNChildren()
    : GridLikeBase1D(), //
      predicate(getp(selectors::mpi_rank, partitioners, {2})),
      skeleton_tree_unfiltered(partition_tree.skeleton_tree()), //
      skeleton_tree(skeleton_tree_unfiltered.prune(predicate))  //
      {};

GridLike1DSize::GridLike1DSize()
    : GridLike1DNChildren(), size_tree(skeleton_tree){};

GridLike1DOffset::GridLike1DOffset()
    : GridLike1DSize(), offset_tree(size_tree){};

} // namespace slow
} // namespace hypercubes
