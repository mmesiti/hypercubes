#include "1D/hbb1D.hpp"
#include <cassert>

namespace hypercubes {
namespace slow {
HBB1D::HBB1D(SizeParity sp, int dimension_, std::string name_, int halo_)
    : Partitioning1D(sp, dimension_, name_), halo(halo_) {
  assert(halo != 0);
  assert(size > 2 * halo);
  limits = std::vector<int>{-halo, 0, halo, size - halo, size, size + halo};
}
int HBB1D::idx_to_coord(int idx, int offset) const {
  return start(idx) + offset;
}
int HBB1D::max_idx_value() const { return 5; };
std::string HBB1D::comments() const { return tuple_to_str(key()); };
std::vector<IndexResult> HBB1D::coord_to_idxs(int relative_x) const {
  for (int i = 0; i < 5; ++i)
    if (start(i) <= relative_x and relative_x < end(i)) {
      return std::vector<IndexResult>{{i, relative_x - start(i), false}};
    }
  return std::vector<IndexResult>();
}
} // namespace slow
} // namespace hypercubes
