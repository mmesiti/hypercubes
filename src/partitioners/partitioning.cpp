#include "partitioners/partitioning.hpp"
#include "utils/utils.hpp"
#include <algorithm>
namespace hypercubes {
namespace slow {
namespace partitioning {

Sizes IPartitioning::idx_to_sizes(int idx) const {
  int child_idx = idx_to_partinfo_kind(idx);
  PartInfoD spd = sub_partinfo_kinds()[child_idx];
  return vtransform(spd,              //
                    [](PartInfo sp) { //
                      return sp.size;
                    });
};
} // namespace partitioning
} // namespace slow
} // namespace hypercubes
