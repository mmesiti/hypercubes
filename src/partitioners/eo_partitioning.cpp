#include "partitioners/eo_partitioning.hpp"
#include "geometry/eo.hpp"
#include "partitioners/partitioning.hpp"
#include "utils/tuple_printer.hpp"
#include "utils/utils.hpp"
#include <set>
#include <stdexcept>

namespace hypercubes {
namespace slow {
namespace partitioning {

EO::EO(const PartInfoD &sp_, const EO::CBFlags &cbflags_,
       const std::string &name_)
    : spd(sp_), cbflags(cbflags_), name(name_) {
  // check that sizes are the same
  if (spd.size() != cbflags.size())
    throw std::invalid_argument(
        name + ": Lengths of sp and cbflags must be the same. " +
        "they are instead, respectively, " + std::to_string(spd.size()) +
        " and " + std::to_string(cbflags.size()));
  // check that Parity::NONE corresponds to cbflag == False
  for (int i = 0; i < cbflags.size(); ++i)
    if ((cbflags[i] and (spd[i].parity == Parity::NONE)) or
        (not cbflags[i] and (spd[i].parity != Parity::NONE)))
      throw std::invalid_argument(
          name + ": Parity and cbflags do not agree." +
          " Index :" + std::to_string(i) +
          " parity: " + std::to_string(static_cast<int>(spd[i].parity)) +
          " cbflag: " + std::to_string(cbflags[i]));

  for (int i = 0; i < cbflags.size(); ++i)
    if (cbflags[i])
      cbsizes.push_back(spd[i].size);

  cumcbsizes = eo::get_cumsizes(cbsizes);
  nsites = *cumcbsizes.rbegin();

  {
    int oddity = 0;
    for (int i = 0; i < spd.size(); ++i)
      if (spd[i].parity == Parity::ODD and
          cbflags[i]) // Redundant for robustness
        oddity++;
    origin_parity = static_cast<Parity>(oddity % 2);
  }
}

Coordinates EO::idx_to_coords(int idx, const Coordinates &offset) const {
  if (dimensionality() + 1 != offset.size())
    throw std::invalid_argument(name + "The length of offset must be " +
                                std::to_string(spd.size() + 1) +
                                ", it is instead " +
                                std::to_string(offset.size()));

  Coordinates res(dimensionality());
  Coordinates sub_res;
  {
    int op = static_cast<int>(origin_parity);
    Parity parity = static_cast<Parity>((idx + op) % 2);
    int idxh = offset[cbflags.size()];
    vector<int> sizes = vtransform(spd,           //
                                   [](auto sp_) { //
                                     return sp_.size;
                                   });
    sub_res = Coordinates(eo::lexeo_idx_to_coord(parity, idxh, cbsizes));
  }
  {
    int cbdim_count = 0;
    for (int i = 0; i < cbflags.size(); ++i)
      if (not cbflags[i])
        res[i] = offset[i];
      else
        res[i] = sub_res[cbdim_count++];
  }
  return res;
}
int EO::idx_to_partinfo_kind(int idx) const {
  if (nsites % 2 == 0)
    return 0;
  else
    return idx;
}
int EO::max_idx_value() const { return 2; }
PartInfosD EO::sub_partinfo_kinds() const {
  int sites_opposite_parity = nsites / 2;
  int sites_origin_parity = nsites - sites_opposite_parity;

  int even_sites, odd_sites;

  if (origin_parity == Parity::EVEN) {
    even_sites = sites_origin_parity;
    odd_sites = sites_opposite_parity;
  }
  if (origin_parity == Parity::ODD) {
    even_sites = sites_opposite_parity;
    odd_sites = sites_origin_parity;
  }

  vector<int> class_sizes;
  if (even_sites == odd_sites)
    class_sizes = vector<int>{even_sites};
  else
    class_sizes = vector<int>{even_sites, odd_sites};

  return vtransform(class_sizes, //
                    [this](int s) {
                      PartInfoD new_sp = spd;
                      for (int i = 0; i < cbflags.size(); ++i)
                        if (cbflags[i])
                          new_sp[i] = PartInfo{1, Parity::NONE};
                      new_sp.push_back(PartInfo{s, Parity::NONE});
                      return new_sp;
                    });
}
std::string EO::get_name() const { return name; }
std::string EO::comments() const { return tuple_to_str(key()); }
vector<IndexResultD> EO::coord_to_idxs(const Coordinates &coord) const {
  vector<IndexResultD> res;
  vector<int> cbcoord;
  for (int i = 0; i < cbflags.size(); ++i)
    if (cbflags[i])
      cbcoord.push_back(coord[i]);
  int relative_eo_idx, idxh;
  std::tie(relative_eo_idx, idxh) = eo::lex_coord_to_eoidx(cbcoord, cumcbsizes);
  int eo_idx = (relative_eo_idx + static_cast<int>(origin_parity)) % 2;

  vector<int> _rests(cbflags.size());
  for (int i = 0; i < cbflags.size(); ++i)
    _rests[i] = cbflags[i] ? 0 : coord[i];
  _rests.push_back(idxh);

  return vector<IndexResultD>{{eo_idx, Coordinates(_rests), false}};
}

int EO::dimensionality() const { return spd.size(); }

} // namespace partitioning
} // namespace slow
} // namespace hypercubes
