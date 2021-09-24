#ifndef PLAIN1D_H_
#define PLAIN1D_H_

#include "partitioning1D.hpp"
#include <tuple>

namespace hypercubes {
namespace slow {
namespace partitioning {

class Plain1D : public Partitioning1D {
public:
  Plain1D(PartInfo sp, int dimension_, std::string name_);
  int idx_to_coord(int idx, int offset) const;
  int max_idx_value() const;
  std::string comments() const;
  std::vector<IndexResult> coord_to_idxs(int relative_x) const;

private:
  auto key() const { return std::make_tuple(size, parity, dimension, name); }
};

} // namespace partitioning
} // namespace slow
} // namespace hypercubes

#endif // PLAIN1D_H_
