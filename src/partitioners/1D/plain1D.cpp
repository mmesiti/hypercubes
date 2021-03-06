#include "partitioners/1D/plain1D.hpp"
#include "utils/tuple_printer.hpp"

namespace hypercubes {
namespace slow {
namespace partitioning {

Plain1D::Plain1D(PartInfo sp, int dimension_, std::string name_)
    : Partitioning1D(sp, dimension_, name_) {
  for (int i = 0; i <= sp.size; ++i)
    limits.push_back(i);
};
int Plain1D::idx_to_coord(int idx, int offset) const {
  if (offset != 0)
    throw std::invalid_argument(name + ": Offset must be zero for Plain1D.");
  if (0 <= idx and idx < size)
    return idx;
  else
    throw std::invalid_argument(name + ": idx=" + std::to_string(idx) +
                                ", not in range [0," + std::to_string(size) +
                                ")");
};
int Plain1D::max_idx_value() const { return size; };
std::string Plain1D::comments() const { return tuple_to_str(key()); };
std::vector<IndexResult> Plain1D::coord_to_idxs(int relative_x) const {
  if (0 <= relative_x and relative_x < size)
    return std::vector<IndexResult>{{relative_x, 0, false}};
  else
    return std::vector<IndexResult>();
};

} // namespace partitioning
} // namespace slow
} // namespace hypercubes
