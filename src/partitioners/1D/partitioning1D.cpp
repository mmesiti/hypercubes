#include "partitioners/1D/partitioning1D.hpp"
#include "utils/print_utils.hpp"
#include <algorithm>
#include <iterator>
#include <set>
#include <sstream>
#include <stdexcept>

namespace hypercubes {
namespace slow {
namespace partitioning {

int Partitioning1D::start(int idx) const { return limits[idx]; }
int Partitioning1D::end(int idx) const { return limits[idx + 1]; }

Parity Partitioning1D::idx_to_start_parity(int idx) const {
  if (parity == Parity::NONE)
    return Parity::NONE;
  else {
    int offset = parity + start(idx);
    // to have a positive quantity
    int new_parity = (offset * offset) % 2;
    return static_cast<Parity>(new_parity);
  }
}
int Partitioning1D::idx_to_size(int idx) const { return end(idx) - start(idx); }

std::vector<PartInfo> Partitioning1D::sub_partinfo_kinds() const {
  std::set<PartInfo> sizes_parities;
  for (int i = 0; i < limits.size() - 1; ++i) {
    sizes_parities.insert(PartInfo{idx_to_size(i), idx_to_start_parity(i)});
  }
  {
    std::vector<PartInfo> sizes_parities_v;
    std::copy(sizes_parities.begin(), sizes_parities.end(),
              std::back_inserter(sizes_parities_v));
    return sizes_parities_v;
  }
}
int Partitioning1D::idx_to_partinfo_kind(int idx) const {
  auto p = idx_to_start_parity(idx);
  auto s = idx_to_size(idx);
  auto sps = sub_partinfo_kinds();
  for (int i = 0; i < sps.size(); ++i)
    if (sps[i] == PartInfo{s, p})
      return i;
  {
    std::stringstream message;
    message << "Child not found."                //
            << " SizeParities: " << sps          //
            << " SizeParity: " << PartInfo{s, p} //
            << " idx: " << idx                   //
            << " max_idx: " << max_idx_value();

    throw std::invalid_argument(message.str());
  }
}
std::string Partitioning1D::get_name() const { return name; }

} // namespace partitioning
} // namespace slow
} // namespace hypercubes
