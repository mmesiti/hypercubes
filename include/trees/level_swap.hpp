#include "partitioners/partitioners.hpp"
#include "utils/utils.hpp"
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <vector>

namespace std {
std::string to_string(std::string s);

} // namespace std

namespace hypercubes {
namespace slow {
namespace internals {

template <class T>
std::vector<int> find_permutation(const std::vector<T> &in,
                                  const std::vector<T> &out) {
  if (in.size() != out.size())
    throw std::invalid_argument(
        std::string("Error, in and out have different lengths."));

  return vtransform(in, [&out](const T &in_el) { //
    auto found = std::find(out.begin(),          //
                           out.end(),            //
                           in_el);
    if (found == out.end()) {
      throw std::invalid_argument(std::string("Error, ") +
                                  std::to_string(in_el) +
                                  " . not found in out vector.");
    }
    return int(found - out.begin());

  });
}

std::vector<std::string>
get_partitioners_names(const partitioners::PartList &partitioners);
} // namespace internals
} // namespace slow
} // namespace hypercubes
