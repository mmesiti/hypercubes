#ifndef EO_PARTITIONING_H_
#define EO_PARTITIONING_H_
#include "geometry.hpp"
#include "partitioning.hpp"
#include <algorithm>
#include <tuple>

namespace hypercubes {
namespace slow {

class EO : public IPartitioning {
public:
  using CBFlags = vector<bool>;

  EO(const SizeParityD &sp_, const CBFlags &cbflags_, const std::string &name_);
  Coordinates idx_to_coords(int idx, const Coordinates &offset) const;
  SizeParitiesD sub_sizeparity_info_list() const;
  int idx_to_child_kind(int idx) const;
  int max_idx_value() const;
  std::string get_name() const;
  std::string comments() const;
  vector<IndexResultD> coord_to_idxs(const Coordinates &coord) const;
  int dimensionality() const;

private:
  const SizeParityD spd;
  const CBFlags cbflags;
  const std::string name;

  std::vector<int> cbsizes, cumcbsizes;
  int nsites;
  Parity origin_parity;
  template <class T> void checkdim(const vector<T> &) const;
  auto key() const { return std::make_tuple(spd, cbflags, name); }
};
} // namespace slow
} // namespace hypercubes

#endif // EO_PARTITIONING_H_
