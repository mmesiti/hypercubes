#ifndef FIXTURES2D_H_
#define FIXTURES2D_H_
#include "api/memory_layout.hpp"
#include "geometry/geometry.hpp"
#include "partitioners/partitioners.hpp"
#include "selectors/partition_predicates.hpp"
#include "trees/partition_tree.hpp"
#include "trees/tree.hpp"

namespace hypercubes {
namespace slow {

struct GridLikeBase2D {

  enum { X, Y, MATROW, EXTRA };
  vector<int> nonspatial_dimensions{MATROW};
  Sizes sizes;
  PartList partitioners;
  PartitionTree<0> partition_tree;

  GridLikeBase2D();
};
struct GridLike2DNChildren : public GridLikeBase2D {
  PartitionPredicate predicate;
  NChildrenTree<0> nchildren_tree_unfiltered;
  NChildrenTree<0> nchildren_tree;
  GridLike2DNChildren();
};
struct GridLike2DSize : public GridLike2DNChildren {
  SizeTree<0> size_tree;
  GridLike2DSize();
};
struct GridLike2DOffset : public GridLike2DSize {
  OffsetTree<0> offset_tree;
  GridLike2DOffset();
};
} // namespace slow
} // namespace hypercubes

#endif // FIXTURES2D_H_
