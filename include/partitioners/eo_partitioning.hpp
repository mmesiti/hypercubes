#ifndef EO_PARTITIONING_H_
#define EO_PARTITIONING_H_
#include "geometry/geometry.hpp"
#include "partitioning.hpp"
#include <algorithm>
#include <tuple>

namespace hypercubes {
namespace slow {
namespace partitioning {

class EO : public IPartitioning {
public:
  using CBFlags = vector<bool>;

  EO(const PartInfoD &sp_, const CBFlags &cbflags_, const std::string &name_);
  Coordinates idx_to_coords(int idx, const Coordinates &offset) const;
  PartInfosD sub_partinfo_kinds() const;
  int idx_to_partinfo_kind(int idx) const;
  int max_idx_value() const;
  std::string get_name() const;
  std::string comments() const;
  vector<IndexResultD> coord_to_idxs(const Coordinates &coord) const;
  int dimensionality() const;

private:
  const PartInfoD spd;
  const CBFlags cbflags;
  const std::string name;

  std::vector<int> cbsizes, cumcbsizes;
  int nsites;
  Parity origin_parity;
  template <class T> void checkdim(const vector<T> &) const;
  auto key() const { return std::make_tuple(spd, cbflags, name); }
};
} // namespace partitioning
} // namespace slow
} // namespace hypercubes

#endif // EO_PARTITIONING_H_
