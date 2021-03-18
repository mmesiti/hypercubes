#ifndef __LEAF_H_
#define __LEAF_H_
#include <cassert>
#include <vector>

#include "base.hpp"

namespace hypercubes {
namespace slow {
namespace partitioning1D {

template <int level> struct Leaf : BasePart<level> {

  using Base = BasePart<level>;
  using Coord = Coordinate<level>;
  using ChildCoord = Coordinate<level + 1>;
  using Partition = Partition1D<level>;

  Leaf(int size) //
      : Base(size){};
  std::vector<int> indices(ChildCoord x) {
    assert(x < Base::end());
    return std::vector<int>({x.value});
  }
  Partition operator[](int i) { return Partition{i, i + 1}; }

  bool is_balanced() { return true; };
  int max_subtree_level() { return level; };
};
// only to declare p(int) as a template for uniformity
/// Function to create a leaf
template <int level> auto leaf(int size) {
  return std::make_shared<Leaf<level>>(size);
}

} // namespace partitioning1D
} // namespace slow
} // namespace hypercubes

#endif // __LEAF_H_
