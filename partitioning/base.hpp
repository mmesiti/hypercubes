#ifndef __PARTITIONING1D_H_
#define __PARTITIONING1D_H_
#include <memory>
#include <ostream>
#include <vector>

namespace hypercubes {
namespace slow {
namespace partitioning1D {

/**
 * Templatized coordinate type
 * not to confuse different levels. */
template <int level> struct Coordinate {
  using Coord = Coordinate<level>;
  using ChildCoord = Coordinate<level + 1>;
  int value;
  Coordinate(int x) : value(x){};
  Coordinate() = delete;
  bool is_zero() { return value == 0; }

#define DEFOP(rtype, OP)                                                       \
  rtype operator OP(Coord m) const { return rtype(value OP m.value); }

  DEFOP(bool, ==)
  DEFOP(bool, <)
  DEFOP(bool, >=)
  DEFOP(int, /)
  DEFOP(ChildCoord, %)
  DEFOP(ChildCoord, -)

#undef DEFOP

  Coord operator+(Coord m) = delete;
  // Coord operator+(ChildCoord m) { return Coord(value + m.value); };

#define DEFINTOP(rtype, OP)                                                    \
  rtype operator OP(int f) const { return rtype(value OP f); }

  DEFINTOP(Coord, *)

#undef DEFINTOP

  struct Offset {
    int delta;
    Offset operator*(int f) { return Offset{delta * f}; }
  };

#define DEFOSOP(OP)                                                            \
  Coord operator OP(Offset d) const { return Coord(value OP d.delta); }

  DEFOSOP(-)
  DEFOSOP(+)

#undef DEFOSOP

  Coord &operator=(int x) {
    value = x;
    return *this;
  }
};

/**
 * Portion representing a piece in the partitioning. */
template <int level> struct Partition1D {
  using ChildCoord = Coordinate<level + 1>;
  ChildCoord start;
  ChildCoord end;
};

/**
 * A hyerarchical partitioning of 1D ranges
 * supporting various partitioning schemes.
 * Using the composite design pattern.
 * */
template <int level> struct BasePart {
  using Coord = Coordinate<level>;
  using ChildCoord = Coordinate<level + 1>;
  using Partition = Partition1D<level>;

  Coord size;
  BasePart(int size_) : size(size_){};
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

template <int level> using BaseP = const std::shared_ptr<BasePart<level>>;
} // namespace partitioning1D
} // namespace slow
} // namespace hypercubes

#endif // __PARTITIONING1D_H_
