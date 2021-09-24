#ifndef HBB1D_H_
#define HBB1D_H_
#include "partitioning1D.hpp"

namespace hypercubes {
namespace slow {
namespace partitioning {

class HBB1D : public Partitioning1D {
public:
  enum BoundaryCondition { PERIODIC, OPEN };
  HBB1D(PartInfo sp, int dimension_, std::string name_, int halo_);
  int idx_to_coord(int idx, int offset) const;
  int max_idx_value() const;
  std::string comments() const;
  std::vector<IndexResult> coord_to_idxs(int relative_x) const;

private:
  int halo;
  auto key() const {
    return std::make_tuple(size, parity, dimension, name, halo);
  }
};

} // namespace partitioning
} // namespace slow
} // namespace hypercubes

#endif // HBB1D_H_
