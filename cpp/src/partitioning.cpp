#include "partitioning.hpp"
#include <algorithm>
namespace hypercubes {
namespace slow {

Sizes IPartitioning::idx_to_sizes(int idx) const {
  int child_idx = idx_to_child_kind(idx);
  SizeParityD spd = sub_sizeparity_info_list()[child_idx];
  Sizes res;
  std::transform(spd.begin(), spd.end(), std::back_inserter(res),
                 [](SizeParity sp) { return sp.size; });
  return res;
};
} // namespace slow
} // namespace hypercubes
