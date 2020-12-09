#ifndef __HYPERCUBES_IMPLEMENTATION_H_
#define __HYPERCUBES_IMPLEMENTATION_H_
#include "hypercubes.hpp"
#include <cmath>

namespace hypercubes {

template <int D>
LocalBlock<D>
LocalLattice<D>::get_block(array<LocalLattice<D>::Portion, D> block_idx) {
  LocalBlock<D> res;
  for (int d = 0; d < D; ++d) {
    res.start = start(block_idx[d], d);
    res.end = end(block_idx[d], d);
  }
  return res;
}

template <int D>
std::vector<typename LocalLattice<D>::PortionBlock>
LocalLattice<D>::all_portions() {
  int total_blocks = std::pow(5, D);

  std::vector<PortionBlock> res(total_blocks);
  for (int iblock = 0; iblock < total_blocks; ++iblock) {
    res[iblock] = get_portion(iblock);
  }
  return res;
}

template <int D>
typename LocalLattice<D>::PortionBlock
LocalLattice<D>::get_portion(int iblock) {
  PortionBlock portion_block;
  for (int d = 0; d < d; ++d) {
    portion_block[d] = iblock % 5;
    iblock /= 5;
  }
  return portion_block;
}

template <int D>
int LocalLattice<D>::start(LocalLattice<D>::Portion p, int dim) {
  int res;
  switch (p) {
  case HALO_MINUS:
    res = -halo_thicknesses[dim];
    break;
  case BORDER_MINUS:
    res = 0;
    break;
  case BULK:
    res = halo_thicknesses[dim];
    break;
  case BORDER_PLUS:
    res = sizes[dim] - halo_thicknesses[dim];
    break;
  case HALO_PLUS:
    res = sizes[dim];
    break;
  }
  return res;
}

template <int D> int LocalLattice<D>::end(LocalLattice<D>::Portion p, int dim) {
  int res;
  switch (p) {
  case HALO_MINUS:
    res = 0;
    break;
  case BORDER_MINUS:
    res = halo_thicknesses[dim];
    break;
  case BULK:
    res = sizes[dim] - halo_thicknesses[dim];
    break;
  case BORDER_PLUS:
    res = sizes[dim];
    break;
  case HALO_PLUS:
    res = sizes[dim] + halo_thicknesses[dim];
    break;
  }
  return res;
}

template <int D>
typename LocalLattice<D>::Portion LocalLattice<D>::portion(int x, int dim) {
  for (int p = HALO_MINUS; p != NUM_PORTIONS; ++p) {
    if (x >= start(p, dim) and x < end(p, dim))
      return p;
  }
  return NUM_PORTIONS;
}

template <int D>
array<typename LocalLattice<D>::Portion, D>
LocalLattice<D>::block_idx(array<int, D> x) {

  array<Portion, D> res;
  for (int d = 0; d < D; ++d)
    res[d] = portion(x[d], d);
  return res;
}
} // namespace hypercubes

#endif // __HYPERCUBES_IMPLEMENTATION_H_
