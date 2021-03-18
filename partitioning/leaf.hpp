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

  Leaf(int id, int size) //
      : Base(id, size){};
  std::vector<int> indices(ChildCoord x) {
    assert(x < Base::end());
    return std::vector<int>({x.value});
  }
  Partition operator[](int i) { return Partition{i, i + 1}; }

  bool is_balanced() { return true; };
  int max_subtree_level() { return level; };
  std::vector<SortablePartitioning::P> children() const {
    return std::vector<SortablePartitioning::P>{};
  }
  bool operator<(const SortablePartitioning &other) const {
    return toposorting::indifferent_relationship(*this, other);
  }
};
template <int level> using Leafp = std::shared_ptr<Leaf<level>>;
template <int level> Leafp<level> leaf(int id, int size) {
  return std::make_shared<Leaf<level>>(id, size);
}

} // namespace partitioning1D
} // namespace slow
} // namespace hypercubes

#endif // __LEAF_H_
