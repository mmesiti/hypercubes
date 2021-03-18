#ifndef __COORDINATES_H_
#define __COORDINATES_H_
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
} // namespace partitioning1D
} // namespace slow
} // namespace hypercubes
#endif // __COORDINATES_H_
