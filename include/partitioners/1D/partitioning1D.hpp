#ifndef PARTITIONING1D_H_
#define PARTITIONING1D_H_
#include "geometry/geometry.hpp"
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace hypercubes {
namespace slow {
class Partitioning1D {
public:
  Partitioning1D(SizeParity sp, int dimension_, std::string name_)
      : size(sp.size), parity(sp.parity), dimension(dimension_), name(name_) {}
  int start(int idx) const;
  int end(int idx) const;
  vector<SizeParity> sub_sizeparity_info_list() const;
  int idx_to_child_kind(int idx) const;
  int idx_to_size(int idx) const;
  virtual int idx_to_coord(int idx, int offset) const = 0;
  virtual int max_idx_value() const = 0;
  std::string get_name() const;
  virtual std::string comments() const = 0;
  virtual vector<IndexResult> coord_to_idxs(int relative_x) const = 0;

protected:
  int size;
  Parity parity;
  int dimension;
  std::string name;

  Parity idx_to_start_parity(int idx) const;
  vector<int> limits;
};

} // namespace slow
} // namespace hypercubes

#endif // PARTITIONING1D_H_
