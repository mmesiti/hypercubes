#ifndef __DIMENSION_IMPLEMENTATION_H_
#define __DIMENSION_IMPLEMENTATION_H_
#include "dimension.hpp"
#include <cassert>
#include <initializer_list>

namespace hypercubes {

Side::Side(int s, int ht) : size(s), halo_thickness(ht) {
  starts[HALO_MINUS] = -halo_thickness;
  starts[BORDER_MINUS] = 0;
  starts[BULK] = +halo_thickness;
  starts[BORDER_PLUS] = size - halo_thickness;
  starts[HALO_PLUS] = size;
  starts[NUM_PORTIONS] = size + halo_thickness;
}

inline int Side::start(Side::Portion p) { return starts[p]; }
inline int Side::end(Side::Portion p) { return starts[p + 1]; }

Side::Portion Side::get_portion(int x) {
  for (Portion p : {HALO_MINUS, BORDER_MINUS, BULK, BORDER_PLUS, HALO_PLUS}) {
    if (x >= start(p) and x < end(p))
      return p;
  }
  return NUM_PORTIONS;
}

Factor::Factor(int _f) : f(_f) {}

Factor Dimension::factor(int i) {
  if (i == 0)
    return Factor(sides[0].size);
  else
    return Factor(sides[i].size / sides[i - 1].size);
}

Factor Factor::operator*(Factor other) { return Factor(other.f * f); }

Dimension::Dimension(container<Side> _sides) : sides(_sides) {
  for (int i = 0; i < sides.size() - 1; ++i)
    assert(sides[i + 1].size % sides[i].size == 0);
};

} // namespace hypercubes

#endif // __DIMENSION_IMPLEMENTATION_H_
