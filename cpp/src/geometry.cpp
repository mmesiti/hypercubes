#include "geometry.hpp"

namespace hypercubes {
namespace slow {

bool operator<(SizeParity p1, SizeParity p2) {
  return p1.size < p2.size or (p1.size == p2.size and p1.parity < p2.parity);
}
bool operator==(SizeParity p1, SizeParity p2) {
  return p1.size == p2.size and p1.parity == p2.parity;
}
bool operator==(IndexResult i1, IndexResult i2) {
  return i1.idx == i2.idx and i1.rest == i2.rest and
         i1.cached_flag == i2.cached_flag;
}
} // namespace slow
} // namespace hypercubes

namespace std {

std::ostream &operator<<(std::ostream &os, hypercubes::slow::Parity p) {

  using P = hypercubes::slow::Parity;
  switch (p) {
  case P::EVEN:
    os << "EVEN";
    break;
  case P::ODD:
    os << "ODD";
    break;
  default:
    os << "NONE";
    break;
  }
  return os;
}
std::ostream &operator<<(std::ostream &os, hypercubes::slow::SizeParity p) {
  os << "<" << p.size << "," << p.parity << ">";
  return os;
}

} // namespace std
