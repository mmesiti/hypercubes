#ifndef Q_H_
#define Q_H_
#include "partitioning1D.hpp"
#include <tuple>

namespace hypercubes {
namespace slow {

class Q1D : public Partitioning1D {
public:
  enum BoundaryCondition { PERIODIC, OPEN };
  Q1D(SizeParity sp, int dimension_, std::string name_, int nparts_, BoundaryCondition bc_);
  int max_idx_value() const;
  std::vector<int> coord_to_idxs(int relative_x) const; // TODO
  std::vector<int> idx_to_coords(int idx, int offset) const; // TODO
  std::string comments() const;

private:
  int nparts;
  BoundaryCondition bc;
  int quotient;
  auto key() const {
    return std::make_tuple(size, parity, dimension, name, nparts, bc);
  }
};

} // namespace slow
} // namespace hypercubes

namespace std {
using BC = hypercubes::slow::Q1D::BoundaryCondition;
std::ostream &operator<<(std::ostream &os, const BC &bc);
bool operator<(const BC &bc1, const BC &bc2);
} // namespace std

#endif // Q_H_
