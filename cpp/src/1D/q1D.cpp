#include "1D/q1D.hpp"
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

Q1DBase::Q1DBase(SizeParity sp, int dimension_, std::string name_, int nparts_)
    : Partitioning1D(sp, dimension_, name_), nparts(nparts_) {
  quotient = size / nparts + (size % nparts) ? 1 : 0;
  assert(size > nparts);
  assert(quotient * (nparts - 1) < size);
  for (int i = 0; i < nparts; ++i)
    limits.push_back(quotient * i);
  limits.push_back(size);
};

std::vector<IndexResult> Q1DBase::coord_to_idxs(int relative_x) const {
  int min_idx, max_idx;
  std::tie(min_idx, max_idx) = idx_limits(relative_x);
  int real_idx = relative_x / quotient;

  std::vector<IndexResult> res;

  auto ghost_limits = [this](int idx) {
    return idx * quotient + (size - nparts * quotient) * (idx / nparts);
  };

  for (int idx = min_idx; idx < max_idx; ++idx) {
    int res_idx = mod(idx, nparts);
    int rest = relative_x - ghost_limits(idx);
    bool cached_flag = (idx != real_idx);
    res.push_back(IndexResult{idx, rest, cached_flag});
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

std::tuple<int,int> Q1DOpen::idx_limits(int relative_x) const{
  return std::make_tuple(0,0); // WRONG
}

std::tuple<int,int> Q1DPeriodic::idx_limits(int relative_x) const{
  return std::make_tuple(0,0); // WRONG
}

} // namespace slow

} // namespace hypercubes
