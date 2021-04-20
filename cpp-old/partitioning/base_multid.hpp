#ifndef __BASE_MULTID_H_
#define __BASE_MULTID_H_

#include "coordinate.hpp"
#include "toposorting.hpp"
#include <array>

namespace hypercubes {
namespace slow {
namespace partitioningND {

using partitioning1D::Coordinate;
using partitioning1D::Partition1D;
/**
 * A hyerarchical partitioning of 1D ranges
 * supporting various partitioning schemes.
 * Using the composite design pattern.
 * */
template <int Dimensionality, int level> struct BasePartND {
  using Coords = std::array<Coordinate<level>, Dimensionality>;
  using PartitionND = std::array<Partition1D<level>, Dimensionality>;
  using ChildCoords = std::array<Coordinate<level + 1>, Dimensionality>;
  using ChildP = std::shared_ptr<BasePartND<Dimensionality, level + 1>>;

  virtual Coords sizes() = 0;
  virtual PartitionND operator[](std::array<int, Dimensionality> is) = 0;

  auto pad() const { return std::string(level * 2, ' '); }
  virtual std::ostream &stream(std::ostream &os) const {
    os << pad();
    for (auto s : sizes())
      os << s << ", ";
    os << ",\n ]";
    return os;
  }
};
} // namespace partitioningND
} // namespace slow
} // namespace hypercubes

#endif // __BASE_MULTID_H_
