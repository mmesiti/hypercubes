#include "partitioners/1D/q1D.hpp"
#include "utils/tuple_printer.hpp"
#include <algorithm>
#include <cassert>

namespace hypercubes {
namespace slow {

// guaranteed to produce a result
// which is positive or equal to zero.
static int mod(int dividend, int divisor) {
  int res = dividend % divisor;
  while (res < 0)
    res += divisor;
  return res;
}

static int floor(int dividend, int divisor) {
  /* The following equations hold:
   * floor(x,y)* y + mod(x,y) == x  // the "python way"
   * (x / y) * y + x % y == x          // the c/c++ way
   * This means
   * floor(x,y) = x / y + (x%y - mod(x,y))/y
   */
  return dividend / divisor +
         (dividend % divisor - mod(dividend, divisor)) / divisor;
}

Q1DBase::Q1DBase(SizeParity sp, int dimension_, std::string name_, int nparts_)
    : Partitioning1D(sp, dimension_, name_), nparts(nparts_) {
  quotient = size / nparts + ((size % nparts) ? 1 : 0);
  assert(size > nparts);
  assert(quotient * (nparts - 1) < size);
  // limits.size() == nparts + 1
  for (int i = 0; i < nparts; ++i)
    limits.push_back(quotient * i);
  limits.push_back(size);
};

std::vector<IndexResult> Q1DBase::coord_to_idxs(int relative_x) const {
  int min_idx, max_idx;
  std::tie(min_idx, max_idx) = idx_limits(relative_x);
  int real_idx = relative_x / quotient;

  vector<IndexResult> res;

  auto ghost_limits = [this](int idx) {
    return idx * quotient + (size - nparts * quotient) * floor(idx, nparts);
  };

  for (int idx = min_idx; idx < max_idx; ++idx) {
    int res_idx = mod(idx, nparts);
    int rest = relative_x - ghost_limits(idx);
    bool cached_flag = (idx != real_idx);
    res.push_back({res_idx, rest, cached_flag});
  }

  return res;
}

int Q1DBase::max_idx_value() const { return nparts; }
int Q1DBase::idx_to_coord(int idx, int offset) const {
  return quotient * idx + offset;
}

std::string Q1DBase::comments() const { return tuple_to_str(key()); }

Q1DPeriodic::Q1DPeriodic(SizeParity sp, int dimension_, std::string name_,
                         int nparts_)
    : Q1DBase(sp, dimension_, name_, nparts_){};
Q1DOpen::Q1DOpen(SizeParity sp, int dimension_, std::string name_, int nparts_)
    : Q1DBase(sp, dimension_, name_, nparts_){};

Q1DBase::BoundaryCondition Q1DPeriodic::bc() const {
  return Q1DBase::BoundaryCondition::PERIODIC;
}
Q1DBase::BoundaryCondition Q1DOpen::bc() const {
  return Q1DBase::BoundaryCondition::OPEN;
}

std::tuple<int, int> Q1DOpen::idx_limits(int relative_x) const {
  int idx_true = relative_x / quotient;
  int min_idx = std::max(0, idx_true - 1);
  int max_idx = std::min(nparts, idx_true + 2);
  return std::make_tuple(min_idx, max_idx);
}

std::tuple<int, int> Q1DPeriodic::idx_limits(int relative_x) const {
  int idx_true = relative_x / quotient;
  int min_idx = idx_true - 1;
  int max_idx = idx_true + 2;
  return std::make_tuple(min_idx, max_idx);
}

} // namespace slow

} // namespace hypercubes

namespace std {
ostream &operator<<(ostream &os,
                    hypercubes::slow::Q1DBase::BoundaryCondition bc) {

  switch (bc) {
  case hypercubes::slow::Q1DBase::BoundaryCondition::OPEN:
    os << "OPEN";
    break;
  case hypercubes::slow::Q1DBase::BoundaryCondition::PERIODIC:
    os << "PERIODIC";
    break;
  }

  return os;
}
} // namespace std
