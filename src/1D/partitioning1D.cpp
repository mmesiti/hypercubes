#include "1D/partitioning1D.hpp"
#include "print_utils.hpp"
#include <algorithm>
#include <iterator>
#include <set>
#include <sstream>
#include <stdexcept>

namespace hypercubes {
namespace slow {
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
  {
    std::stringstream message;
    message << "Child not found."                  //
            << " SizeParities: " << sps            //
            << " SizeParity: " << SizeParity{s, p} //
            << " idx: " << idx                     //
            << " max_idx: " << max_idx_value();

    throw std::invalid_argument(message.str());
  }
}
std::string Partitioning1D::get_name() const { return name; }

} // namespace slow
} // namespace hypercubes
