#ifndef EO_PARTITIONING_H_
#define EO_PARTITIONING_H_
#include "geometry.hpp"
#include "partitioning.hpp"
#include <algorithm>

namespace hypercubes {
namespace slow {

class EO : public IPartitioning {
public:
  using CBFlags = vector<bool>;

  EO(const SizeParities &sp_, const CBFlags &cbflags_,
     const std::string &name_);
  Coordinates idx_to_coords(int idx, const Coordinates &offset) const;
  Sizes idx_to_sizes(int idx, const Sizes &sizes) const;
  SizeParitiesD sub_sizeparity_info_list() const;
  int idx_to_child_kind(int idx) const;
  int max_idx_value() const;
  std::string comments() const;
  vector<IndexResultD> coord_to_idxs(const Coordinates &coord) const;

private:
  const SizeParities sp;
  const CBFlags cbflags;
  const std::string name;

  std::vector<int> cbsizes, cumcbsizes;
  int nsites;
  Parity origin_parity;
  template <class T> void checkdim(const vector<T> &);
};
} // namespace slow
} // namespace hypercubes

#endif // EO_PARTITIONING_H_
