#include "dimension.hpp"
#include <array>
#include <functional>
#include <numeric>
#include <vector>

namespace hypercubes {
using std::array;

template <int Dimensionality> class LocalBlock {
  array<int, Dimensionality> starts;
  array<int, Dimensionality> ends;
  virtual int volume() {
    int res = 1;
    for (int d = 0; d < Dimensionality; ++d) {
      res *= ends[d] - starts[d];
    }
    return res;
  }
};

template <int Dimensionality> struct LocalEOBlock : LocalBlock<Dimensionality> {
  array<bool, Dimensionality> is_dim_checherboarded;
  int checkerboard_shift_dimension;
  enum Type { LOCAL_EVEN, LOCAL_ODD, NUM_TYPES };
  Type type;
};

template <int Dimensionality> class LocalLattice {
  using NDPortion = array<Dimension::Portion, Dimensionality>;
  using Location = array<int, Dimensionality>;

  array<Dimension, Dimensionality> dimensions;

  NDPortion get_nd_portion(int i);
  NDPortion get_nd_portion(Location x);
  LocalBlock<Dimensionality> get_local_block(NDPortion block_idx);

  std::vector<NDPortion> all_portions();
};

} // namespace hypercubes
