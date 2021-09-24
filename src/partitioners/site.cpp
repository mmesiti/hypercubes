#include "partitioners/site.hpp"
#include <exception>
#include <stdexcept>

namespace hypercubes {
namespace slow {
namespace partitioning {

Site::Site(const PartInfoD &sp_) : dimension(sp_.size()) {}

void Site::check_idx(int idx) const {
  if (idx != 0)
    throw std::invalid_argument(
        std::string("For Site idx must be 0, not " + std::to_string(idx)));
}
Coordinates Site::idx_to_coords(int idx, const Coordinates &offset) const {
  check_idx(idx);
  return Coordinates(dimensionality(), 0);
}
Sizes Site::idx_to_sizes(int idx) const {
  check_idx(idx);
  return Sizes(dimensionality(), 0);
}
PartInfosD Site::sub_partinfo_kinds() const { return PartInfosD(); }
int Site::idx_to_partinfo_kind(int idx) const {
  check_idx(idx);
  return 0;
}

int Site::max_idx_value() const { return 1; }
bool Site::is_leaf() const { return true; }
std::string Site::get_name() const { return "Site"; }
std::string Site::comments() const { return ""; }
vector<IndexResultD> Site::coord_to_idxs(const Coordinates &coord) const {
  return {{0, Coordinates(dimensionality(), 0), false}};
};
int Site::dimensionality() const { return dimension; }

} // namespace partitioning
} // namespace slow
} // namespace hypercubes
