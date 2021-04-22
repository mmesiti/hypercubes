#include "1D/q1D.hpp"
#include <cassert>

namespace hypercubes {
namespace slow {

Q1D::Q1D(SizeParity sp, int dimension_, std::string name_, int nparts_,
         BoundaryCondition bc_)
    : Partitioning1D(sp, dimension_, name_), nparts(nparts_), bc(bc_) {
  quotient = size / nparts + (size % nparts) ? 1 : 0;
  assert(size > nparts);
  assert(quotient * (nparts - 1) < size);
  for (int i = 0; i < nparts; ++i)
    limits.push_back(quotient * i);
  limits.push_back(size);
};
int Q1D::max_idx_value() const { return nparts; }

std::string Q1D::comments() const { return tuple_to_str(key()); }

} // namespace slow

} // namespace hypercubes

namespace std {
std::ostream &operator<<(std::ostream &os, const BC &bc) {
  std::string res;
  switch (bc) {
  case BC::OPEN:
    res = "OPEN";
    break;
  case BC::PERIODIC:
    res = "PERIODIC";
    break;
  }
  os << res;
  return os;
}
bool operator<(const BC &bc1, const BC &bc2) {
  return static_cast<int>(bc1) < static_cast<int>(bc2);
}
} // namespace std
