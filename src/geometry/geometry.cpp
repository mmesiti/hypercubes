#include "geometry/geometry.hpp"
#include <algorithm>

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

Coordinates up(Coordinates coords, //
               int dir) {
  Coordinates res(coords);
  res[dir]++;
  return res;
}
Coordinates down(Coordinates coords, //
                 int dir) {
  Coordinates res(coords);
  res[dir]--;
  return res;
}
Coordinates up(Coordinates coords,            //
               Sizes sizes,                   //
               vector<BoundaryCondition> bcs, //
               int dir) {

  Coordinates res(coords);
  res[dir]++;
  switch (bcs[dir]) {
  case BoundaryCondition::OPEN:
    if (res[dir] > sizes[dir] - 1)
      return Coordinates{};
  case BoundaryCondition::PERIODIC:
    while (res[dir] > sizes[dir] - 1)
      res[dir] -= sizes[dir];
    return res;
  default:
    return res;
  }
}
Coordinates down(Coordinates coords,            //
                 Sizes sizes,                   //
                 vector<BoundaryCondition> bcs, //
                 int dir) {
  Coordinates res(coords);
  res[dir]--;
  switch (bcs[dir]) {
  case BoundaryCondition::OPEN:
    if (res[dir] < 0)
      return Coordinates{};
  case BoundaryCondition::PERIODIC:
    while (res[dir] < 0)
      res[dir] += sizes[dir];
    return res;
  default:
    return res;
  }
}

SizeParityD add_parity(Sizes sizes, vector<int> nonspatial_dimensions) {

  SizeParityD res;
  for (int i = 0; i < sizes.size(); ++i)
    res.push_back({sizes[i], (std::find(nonspatial_dimensions.begin(), //
                                        nonspatial_dimensions.end(),   //
                                        i) == nonspatial_dimensions.end())
                                 ?

                                 Parity::EVEN
                                 : Parity::NONE});
  return res;
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
