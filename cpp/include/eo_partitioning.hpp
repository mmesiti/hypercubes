#ifndef EO_PARTITIONING_H_
#define EO_PARTITIONING_H_
#include "partitioning.hpp"
#include <algorithm>
// TODO

namespace hypercubes {
namespace slow {

template <int Dimensions> class EO : public IPartitioning<Dimensions> {

private:
  GeomInfos gs; // parities and sizes
  CBFlags cbflags;
  std::string name;

  std::vector<int> cbsizes, cumcbsizes;
  int nsites;
  Parity origin_parity;

  // TODO: all_eos (or implement somewhere else)
public:
  using Coordinates = typename IPartitioning<Dimensions>::Coordinates;
  using Sizes = typename IPartitioning<Dimensions>::Sizes;
  using GeomInfos = typename IPartitioning<Dimensions>::GeomInfos;
  using CBFlags = std::array<bool, Dimensions>;

  EO(const GeomInfos &gs_, const CBFlags &cbflags_, const std::string &name_)
      : gs(gs_), cbflags(cbflags_), name(name_) {

    for (int i = 0; i < cbflags.size; ++i)
      if cbflags
        [i] cbsizes.push_back(gs[i].size);
  }
};
} // namespace slow
} // namespace hypercubes

#endif // EO_PARTITIONING_H_
