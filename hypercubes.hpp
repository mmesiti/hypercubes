#include <array>
#include <functional>
#include <numeric>
#include <vector>

namespace hypercubes {
using std::array;

template <int Dimension> class LocalBlock {
  array<int, Dimension> starts;
  array<int, Dimension> ends;
  virtual int volume() {
    int res = 1;
    for (int d = 0; d < Dimension; ++d) {
      res *= ends[d] - starts[d];
    }
    return res;
  }
};

template <int Dimension> struct EOBlock : class Block<Dimension> {
  array<bool, Dimension> is_dim_checherboarded;
  enum Type { LOCAL_EVEN, LOCAL_ODD, NUM_TYPES };
  Type type;
};

struct FlattenedContiguousBlock {
  int start, end;
};

template <int Dimension> class LocalLattice {
  enum Portion {
    HALO_MINUS,
    BORDER_MINUS,
    BULK,
    BORDER_PLUS,
    HALO_PLUS,
    NUM_PORTIONS
  };
  using PortionBlock = array<Portion, Dimension>;

  array<int, Dimension> sizes;
  array<int, Dimension> halo_thicknesses;

  int start(Portion p, int dimension);

  int end(Portion p, int dimension);

  Portion portion(int x, int dim);

  array<Portion, Dimension> block_idx(array<int, Dimension> x);

  LocalBlock<Dimension> get_block(PortionBlock block_idx);

  std::vector<PortionBlock> all_portions();

  PortionBlock get_portion(int i);
};

} // namespace hypercubes
