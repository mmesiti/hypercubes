#include "geometry.hpp"

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
bool operator<(SizeParity p1, SizeParity p2) {
  return p1.size < p2.size or (p1.size == p2.size and p1.parity < p2.parity);
}
} // namespace std
