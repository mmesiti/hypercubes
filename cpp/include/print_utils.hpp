#ifndef __PRINT_UTILS_H_
#define __PRINT_UTILS_H_
#include <ostream>
#include <vector>

namespace std {

template <class T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v) {
  os << std::string("[");
  for (int i = 0; i < v.size() - 1; ++i)
    os << v[i] << ", ";
  os << *v.rbegin() << "]";

  return os;
}

} // namespace std
#endif // __PRINT_UTILS_H_
