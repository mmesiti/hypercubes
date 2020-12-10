#ifndef __DIMENSION_IMPLEMENTATION_H_
#define __DIMENSION_IMPLEMENTATION_H_
#include "dimension.hpp"
#include <initializer_list>

namespace hypercubes {

Dimension::Dimension(int s, int ht) : size(s), halo_thickness(ht) {
  starts[HALO_MINUS] = -halo_thickness;
  starts[BORDER_MINUS] = 0;
  starts[BULK] = +halo_thickness;
  starts[BORDER_PLUS] = size - halo_thickness;
  starts[HALO_PLUS] = size;
  starts[NUM_PORTIONS] = size + halo_thickness;
}

int Dimension::start(Dimension::Portion p) { return starts[p]; }
int Dimension::end(Dimension::Portion p) { return starts[p + 1]; }

Dimension::Portion Dimension::get_portion(int x) {
  for (Portion p : {HALO_MINUS, BORDER_MINUS, BULK, BORDER_PLUS, HALO_PLUS}) {
    if (x >= start(p) and x < end(p))
      return p;
  }
  return NUM_PORTIONS;
}

} // namespace hypercubes

#endif // __DIMENSION_IMPLEMENTATION_H_
