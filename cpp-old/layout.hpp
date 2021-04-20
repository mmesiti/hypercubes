#ifndef __LAYOUT_H_
#define __LAYOUT_H_

#include "dimension.hpp"
#include <array>
#include <tuple>

namespace hypercubes {

template <int dimensionality> class Layout {
  std::array<Dimension, dimensionality> dimensions;
  int max_halo_dimensionality;
  using Direction = int;
  container<std::tuple<Side, Direction>> side_permutation;
  container<Factor> factors;
  container<Factor> strides;
};
} // namespace hypercubes

#endif // __LAYOUT_H_
