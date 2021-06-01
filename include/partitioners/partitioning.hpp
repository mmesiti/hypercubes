#ifndef PARTITIONING_H_
#define PARTITIONING_H_
#include "geometry/geometry.hpp"
#include <string>
#include <vector>

namespace hypercubes {
namespace slow {

class IPartitioning {
public:
  virtual Coordinates idx_to_coords(int idx,
                                    const Coordinates &offset) const = 0;
  Sizes idx_to_sizes(int idx) const;
  virtual SizeParitiesD sub_sizeparity_info_list() const = 0;
  virtual int idx_to_child_kind(int idx) const = 0;
  virtual int max_idx_value() const = 0;
  virtual std::string get_name() const = 0;
  virtual std::string comments() const = 0;
  virtual vector<IndexResultD>
  coord_to_idxs(const Coordinates &coord) const = 0;
  virtual int dimensionality() const = 0;
};

} // namespace slow
} // namespace hypercubes

#endif // PARTITIONING_H_
