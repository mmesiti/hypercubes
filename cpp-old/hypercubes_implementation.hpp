#ifndef __HYPERCUBES_IMPLEMENTATION_H_
#define __HYPERCUBES_IMPLEMENTATION_H_
#include "dimension.hpp"
#include "hypercubes.hpp"
#include <cmath>

namespace hypercubes {

template <int D> using NDPortion = typename LocalLattice<D>::NDPortion;

template <int D>
LocalBlock<D> LocalLattice<D>::get_local_block(NDPortion block_idx) {
  LocalBlock<D> res;
  for (int d = 0; d < D; ++d) {
    res.starts[d] = dimensions[d].start(block_idx[d]);
    res.ends[d] = dimensions[d].end(block_idx[d]);
  }
  return res;
}

template <int D> std::vector<NDPortion<D>> LocalLattice<D>::all_portions() {
  int total_blocks = std::pow(5, D);

  std::vector<NDPortion> res(total_blocks);
  for (int iblock = 0; iblock < total_blocks; ++iblock) {
    res[iblock] = get_nd_portion(iblock);
  }
  return res;
}

template <int D> NDPortion<D> LocalLattice<D>::get_nd_portion(int iblock) {
  NDPortion portion_block;
  for (int d = 0; d < D; ++d) {
    portion_block[d] = iblock % 5;
    iblock /= 5;
  }
  return portion_block;
}

template <int D> NDPortion<D> LocalLattice<D>::get_nd_portion(Location x) {

  NDPortion res;
  for (int d = 0; d < D; ++d)
    res[d] = dimensions[d].portion(x[d]);
  return res;
}
} // namespace hypercubes

#endif // __HYPERCUBES_IMPLEMENTATION_H_
