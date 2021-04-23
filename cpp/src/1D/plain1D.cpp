#include "1D/plain1D.hpp"

namespace hypercubes {
namespace slow {
Plain1D::Plain1D(SizeParity sp, int dimension_, std::string name_)
    : Partitioning1D(sp, dimension_, name_) {
  limits = std::vector<int>{0, 1};
};
int Plain1D::idx_to_coord(int idx, int offset) const {
  if (offset != 0)
    throw std::runtime_error(name + ": Offset must be zero for Plain1D.");
  if (0 <= idx and idx < size)
    return idx;
  else
    throw std::runtime_error(name + ": idx=" + std::to_string(idx) +
                             ", not in range [0," + std::to_string(size) + ")");
};
int Plain1D::max_idx_value() const { return size; };
std::string Plain1D::comments() const { return ""; };
std::vector<IndexResult> Plain1D::coord_to_idxs(int relative_x) const {
  if (0 <= relative_x and relative_x < size)
    return std::vector<IndexResult>{{relative_x, 0, false}};
  else
    return std::vector<IndexResult>();
};

} // namespace slow
} // namespace hypercubes