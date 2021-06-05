#include "partitioners/partitioning.hpp"
#include "utils/utils.hpp"
#include <algorithm>
namespace hypercubes {
namespace slow {
namespace partitioning {

Sizes IPartitioning::idx_to_sizes(int idx) const {
  int child_idx = idx_to_child_kind(idx);
  SizeParityD spd = sub_sizeparity_info_list()[child_idx];
  return vtransform(spd,                //
                    [](SizeParity sp) { //
                      return sp.size;
                    });
};
} // namespace partitioning
} // namespace slow
} // namespace hypercubes
