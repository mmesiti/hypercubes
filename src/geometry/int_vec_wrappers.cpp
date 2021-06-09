#include "geometry/int_vec_wrappers.hpp"
#include "utils/print_utils.hpp"

using namespace hypercubes::slow;
using namespace std;

std::ostream &std::operator<<(ostream &os, const Coordinates &cs) {
  os << cs.coords;
  return os;
}
std::ostream &std::operator<<(ostream &os, const Indices &idxs) {
  os << idxs.idxs;
  return os;
}

namespace hypercubes {
namespace slow {

#define INTVEC_BOILERPLATE(CLASSNAME, WRAPPEDNAME)                             \
  int &CLASSNAME::operator[](size_t d) { return WRAPPEDNAME[d]; }              \
  int CLASSNAME::operator[](size_t d) const { return WRAPPEDNAME[d]; }         \
  size_t CLASSNAME::size() const { return WRAPPEDNAME.size(); }                \
  bool CLASSNAME::operator==(const CLASSNAME &other) const {                   \
    return WRAPPEDNAME == other.WRAPPEDNAME;                                   \
  }                                                                            \
  bool CLASSNAME::operator!=(const CLASSNAME &other) const {                   \
    return WRAPPEDNAME != other.WRAPPEDNAME;                                   \
  }                                                                            \
  CLASSNAME::CLASSNAME(std::initializer_list<int> il) : WRAPPEDNAME(il){};
// coordinates
INTVEC_BOILERPLATE(Coordinates, coords);
// indices
INTVEC_BOILERPLATE(Indices, idxs);
#undef INTVEC_BOILERPLATE

void Indices::push_back(int idx) { idxs.push_back(idx); }

Indices append(Indices idxs, int idx) {
  idxs.push_back(idx);
  return idxs;
}

Indices append(int i, const Indices &idxs) {
  vector<int> res{i};
  std::copy(idxs.idxs.begin(), idxs.idxs.end(), std::back_inserter(res));
  return Indices(res);
}

Indices tail(const Indices &idxs) {
  Indices new_idxs;
  std::copy(idxs.begin() + 1, idxs.end(), std::back_inserter(new_idxs.idxs));
  return new_idxs;
}

} // namespace slow
} // namespace hypercubes
