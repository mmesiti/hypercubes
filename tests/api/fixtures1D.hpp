#ifndef FIXTURES1D_H_
#define FIXTURES1D_H_
#include "api/memory_layout.hpp"
#include "geometry/geometry.hpp"
#include "partitioners/partitioners.hpp"
#include "selectors/partition_predicates.hpp"
#include "trees/partition_tree.hpp"
#include "trees/tree.hpp"

// 1D
namespace hypercubes {
namespace slow {

struct GridLikeBase1D {
  enum { X, MATROW, EXTRA };
  vector<int> nonspatial_dimensions{MATROW};
  Sizes sizes;
  PartList partitioners;
  PartitionTree<0> partition_tree;
  GridLikeBase1D();
};
struct GridLike1DNChildren : public GridLikeBase1D {
  PartitionPredicate predicate;
  NChildrenTree<0> nchildren_tree_unfiltered;
  NChildrenTree<0> nchildren_tree;
  GridLike1DNChildren();
};

struct GridLike1DSize : public GridLike1DNChildren {
  SizeTree<0> size_tree;
  GridLike1DSize();
};
struct GridLike1DOffset : public GridLike1DSize {
  OffsetTree<0> offset_tree;
  GridLike1DOffset();
};

} // namespace slow
} // namespace hypercubes
#endif // FIXTURES1D_H_
