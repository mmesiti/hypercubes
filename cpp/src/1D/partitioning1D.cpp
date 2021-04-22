#include "1D/partitioning1D.hpp"
#include <algorithm>
#include <iterator>
#include <set>

namespace hypercubes {
namespace slow {
int Partitioning1D::start(int idx) const { return limits[idx]; }
int Partitioning1D::end(int idx) const { return limits[idx + 1]; }

Parity Partitioning1D::idx_to_start_parity(int idx) const {
  return (parity == Parity::NONE)
             ? Parity::NONE
             : static_cast<Parity>((parity + start(idx)) % 2);
}
int Partitioning1D::idx_to_size(int idx) const { return end(idx) - start(idx); }

std::vector<SizeParity> Partitioning1D::sub_sizeparity_info_list() const {
  std::set<SizeParity> sizes_parities;
  for (int i = 0; i < limits.size() - 1; ++i) {
    sizes_parities.insert(SizeParity{idx_to_size(i), idx_to_start_parity(i)});
  }
  {
    std::vector<SizeParity> sizes_parities_v;
    std::copy(sizes_parities.begin(), sizes_parities.end(),
              std::back_inserter(sizes_parities_v));
    return sizes_parities_v;
  }
}
int Partitioning1D::idx_to_child_kind(int idx) const {
  auto p = idx_to_start_parity(idx);
  auto s = idx_to_size(idx);
  auto sps = sub_sizeparity_info_list();
  for (int i = 0; i < sps.size(); ++i)
    if (sps[i] == SizeParity{s, p})
      return i;
  return -1;
}

} // namespace slow
} // namespace hypercubes
