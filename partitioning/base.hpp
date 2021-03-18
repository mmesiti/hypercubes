#ifndef __PARTITIONING1D_H_
#define __PARTITIONING1D_H_
#include <memory>
#include <ostream>
#include <vector>

#include "coordinate.hpp"
#include "toposorting.hpp"

namespace hypercubes {
namespace slow {
namespace partitioning1D {

using toposorting::SortablePartitioning;
/**
 * A hyerarchical partitioning of 1D ranges
 * supporting various partitioning schemes.
 * Using the composite design pattern.
 * */
template <int level> struct BasePart : SortablePartitioning {
  using Coord = Coordinate<level>;
  using ChildCoord = Coordinate<level + 1>;
  using Partition = Partition1D<level>;
  using P = const std::shared_ptr<BasePart>;

  Coord size;
  BasePart(int id, int size_) : SortablePartitioning(id), size(size_){};
  ChildCoord start() { return 0; }
  ChildCoord end() { return ChildCoord(size.value); }
  /**
   * Given a location along the axis,
   * tell to which partitions it belongs
   * down the tree.
   * */
  bool not_empty() { return not size.is_zero(); }
  virtual std::vector<int> indices(ChildCoord x) = 0;
  virtual std::ostream &stream(std::ostream &os) const {
    os << "size:" << size;
    return os;
  }
  virtual Partition operator[](int i) = 0;
  virtual bool is_balanced() = 0;
  virtual int max_subtree_level() = 0;
};

template <int level>
std::ostream &operator<<(std::ostream &os, const BasePart<level> &partition) {
  return partition.stream(os);
}

template <int level>
std::ostream &operator<<(std::ostream &os, const Coordinate<level> &x) {
  os << x.value;
  return os;
}

} // namespace partitioning1D
} // namespace slow
} // namespace hypercubes

#endif // __PARTITIONING1D_H_
