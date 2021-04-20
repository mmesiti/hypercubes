#ifndef __EVEN_ODD_H_
#define __EVEN_ODD_H_

#include <algorithm>
#include <array>
#include <numeric>

namespace hypercubes {

template <int Dimensionality>
std::array<int, Dimensionality + 1>
toEO(const std::array<int, Dimensionality> coordinates_in,
     const int checkerboarded_direction = 0) {

  int parity =
      std::accumulate(coordinates_in.begin(), coordinates_in.end(), 0) % 2;

  std::array<int, Dimensionality + 1> coordinates_out_eo;
  std::copy(coordinates_in.begin(), coordinates_in.end(),
            coordinates_out_eo.begin());
  coordinates_out_eo[checkerboarded_direction] /= 2;
  *coordinates_out_eo.rbegin() = parity;
  return coordinates_out_eo;
}

template <int Dimensionality>
std::array<int, Dimensionality>
fromEO(const std::array<int, Dimensionality + 1> coordinates_in_eo,
       const int checkerboarded_direction = 0) {

  int parity = *coordinates_in_eo.rbegin();
  std::array<int, Dimensionality> coordinates_out;

  std::copy(coordinates_in_eo.begin(), coordinates_in_eo.end() - 1,
            coordinates_out.begin());

  coordinates_out[checkerboarded_direction] *= 2;

  int parity_fix =
      (std::accumulate(coordinates_out.begin(), coordinates_out.end(), 0) -
       parity) %
      2;

  coordinates_out[checkerboarded_direction] += parity_fix;

  return coordinates_out;
}

} // namespace hypercubes

#endif // __EVEN_ODD_H_
