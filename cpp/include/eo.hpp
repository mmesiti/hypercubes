#ifndef EO_H_
#define EO_H_
#include "partitioning.hpp"
#include <cassert>
#include <tuple>

namespace hypercubes {
namespace slow {
namespace eo {

using IntList = std::vector<int>;
IntList get_cumsizes(const IntList &sizes);
IntList lex_idx_to_coord(int lexi, const IntList &sizes);

int lex_coord_to_idx(const IntList &coords, const IntList &cumsizes);

Parity coord_to_eo(const IntList &coords);

std::tuple<Parity, int> lex_coord_to_eoidx(const IntList &coords,
                                           const IntList &cumsizes);

IntList lexeo_idx_to_coord(Parity eoflag, int idxh, const IntList &sizes);
void ensuresize(IntList &l, int s);
} // namespace eo
} // namespace slow
} // namespace hypercubes

#endif // EO_H_
