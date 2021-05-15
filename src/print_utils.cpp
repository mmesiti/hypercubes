#include "print_utils.hpp"

namespace std {
template <>
std::ostream &operator<<(std::ostream &os, const std::vector<bool> &v) {
  for (bool s : v)
    os << (s ? "T" : "F");
  return os;
}
} // namespace std
