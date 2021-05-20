#include "level_swap.hpp"
#include "utils.hpp"

namespace std {
std::string to_string(std::string s) { return s; }

} // namespace std
namespace hypercubes {
namespace slow {

std::vector<std::string>
get_partitioners_names(const partitioners::PartList &partitioners) {
  return vtransform(partitioners, [](auto p) { return p->get_name(); });
}
} // namespace slow
} // namespace hypercubes
