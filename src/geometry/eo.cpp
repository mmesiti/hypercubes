#include "geometry/eo.hpp"

namespace hypercubes {
namespace slow {
namespace eo {

using IntList = std::vector<int>;
IntList get_cumsizes(const IntList &sizes) {
  IntList res;
  ensuresize(res, sizes.size() + 1);
  res[0] = 1;
  for (int i = 0; i < sizes.size(); ++i)
    res[i + 1] = res[i] * sizes[i];
  return res;
}

IntList lex_idx_to_coord(int lexi, const IntList &sizes) {
  IntList coordinates;
  ensuresize(coordinates, sizes.size());
  int i = lexi;
  for (int d = 0; d < sizes.size(); ++d) {
    coordinates[d] = i % sizes[d];
    i = i / sizes[d];
  }

  return coordinates;
}

Parity coord_to_eo(const IntList &coords) {
  int sum = 0;
  for (auto x : coords)
    sum += x;
  return (sum % 2 == 0) ? Parity::EVEN : Parity::ODD;
}

int lex_coord_to_idx(const IntList &coords, const IntList &cumsizes) {
  int idx = 0;
  for (int i = 0; i < coords.size(); ++i)
    idx += coords[i] * cumsizes[i];
  return idx;
}

std::tuple<Parity, int> lex_coord_to_eoidx(const IntList &coords,
                                           const IntList &cumsizes) {
  int idxh = lex_coord_to_idx(coords, cumsizes) / 2;
  Parity p = coord_to_eo(coords);

  return std::make_tuple(p, idxh);
}

IntList lexeo_idx_to_coord(Parity eoflag, int idxh, const IntList &sizes) {
  auto coordinates = lex_idx_to_coord(2 * idxh, sizes);
  if (eoflag == coord_to_eo(coordinates))
    return coordinates;
  else
    return lex_idx_to_coord(2 * idxh + 1, sizes);
}

void ensuresize(IntList &l, int s) { l.resize(s); }

} // namespace eo
} // namespace slow
} // namespace hypercubes
