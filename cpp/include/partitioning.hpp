#ifndef PARTITIONING_H_
#define PARTITIONING_H_
#include <array>
#include <string>
#include <vector>

namespace hypercubes {
namespace slow {

struct IndexResult {
  int idx;
  int rest;
  int cached_flag;
};

enum Parity { EVEN, ODD };

struct SizeParity {
  int size;
  Parity parity;
};

template <int Dimensions> class IPartitioning {
public:
  using Coordinates = std::array<int, Dimensions>;
  using Sizes = std::array<int, Dimensions>;
  using GeomInfos = std::array<SizeParity, Dimensions>;

  virtual std::array<SizeParity, Dimensions> sub_geom_info_list() = 0;
  virtual std::vector<IndexResult> coord_to_idxs(Coordinates coord) = 0;
  virtual Coordinates idx_to_coords(int idx, Coordinates offset) = 0;
  virtual Sizes idx_to_sizes(int idx, Coordinates offset) = 0;
  virtual int idx_to_child_kind(int idx) = 0;
  virtual int max_idx_value() = 0;
  virtual std::string comments() = 0;
};

} // namespace slow
} // namespace hypercubes

#endif // PARTITIONING_H_
