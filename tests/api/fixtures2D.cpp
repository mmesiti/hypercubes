#include "fixtures2D.hpp"
#include "api/memory_layout.hpp"

namespace hypercubes {
namespace slow {

namespace pm = hypercubes::slow::partitioner_makers;
GridLikeBase2D::GridLikeBase2D()
    : sizes({48, 48, 3}), partitioners({
                              pm::QPeriodic("MPI X", X, 4),      // 0
                              pm::QPeriodic("MPI Y", Y, 4),      // 1
                              pm::QOpen("Vector X", X, 2),       // 2
                              pm::QOpen("Vector Y", Y, 2),       // 3
                              pm::HBB("Halo X", X, 1),           // 4
                              pm::HBB("Halo Y", Y, 1),           // 5
                              pm::EO("EO", {true, true, false}), // 6
                              pm::Plain("Local-matrow", MATROW), // 7
                              pm::Plain("Extra", EXTRA),         // 8
                              pm::Site()                         // 9
                          }),
      partition_tree(sizes, partitioners, nonspatial_dimensions) {}

GridLike2DNChildren::GridLike2DNChildren()
    : GridLikeBase2D(), //
      predicate(getp(mpi_rank, partitioners, {2, 1})),
      nchildren_tree(NChildrenTree(partition_tree).prune(predicate)){};
GridLike2DSize::GridLike2DSize()
    : GridLike2DNChildren(), size_tree(nchildren_tree){};
GridLike2DOffset::GridLike2DOffset()
    : GridLike2DSize(), offset_tree(size_tree){};
} // namespace slow
} // namespace hypercubes
