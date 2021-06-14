#ifndef FIXTURES4D_H_
#define FIXTURES4D_H_

#include "api/memory_layout.hpp"
#include "geometry/geometry.hpp"
#include "partitioners/partitioners.hpp"
#include "selectors/partition_predicates.hpp"
#include "trees/partition_tree.hpp"
#include "trees/tree.hpp"
namespace hypercubes {
namespace slow {

struct GridLikeBase {

  enum { X, Y, Z, T, MATROW, MATCOL, EXTRA };
  vector<int> nonspatial_dimensions{MATROW, MATCOL};
  Sizes sizes;
  PartList partitioners;
  PartitionTree<0> partition_tree;

  GridLikeBase();
};
struct GridLikeNChildren : public GridLikeBase {
  PartitionPredicate predicate;
  NChildrenTree<0> nchildren_tree;
  GridLikeNChildren();
};
struct GridLikeSize : public GridLikeNChildren {
  SizeTree<0> size_tree;
  GridLikeSize();
};
struct GridLikeOffset : public GridLikeSize {
  OffsetTree<0> offset_tree;
  GridLikeOffset();
};

} // namespace slow
} // namespace hypercubes
#endif // FIXTURES4D_H_
