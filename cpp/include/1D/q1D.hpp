#ifndef Q_H_
#define Q_H_
#include "partitioning1D.hpp"
#include <tuple>

namespace hypercubes {
namespace slow {

class Q1DBase : public Partitioning1D {
public:
  enum BoundaryCondition { PERIODIC, OPEN };
  Q1DBase(SizeParity sp, int dimension_, std::string name_, int nparts_);
  std::vector<IndexResult> coord_to_idxs(int relative_x) const;
  int max_idx_value() const;
  int idx_to_coord(int idx, int offset) const;
  std::string comments() const;

protected:
  int nparts;
  int quotient;

private:
  auto key() const {
    using BC = Q1DBase::BoundaryCondition;
    return std::make_tuple(size, parity, dimension, name, nparts, bc());
  }
  virtual std::tuple<int, int> idx_limits(int relative_x) const = 0;
  virtual BoundaryCondition bc() const = 0;
};

class Q1DPeriodic : public Q1DBase {
public:
  Q1DPeriodic(SizeParity sp, int dimension_, std::string name_, int nparts_);

private:
  std::tuple<int, int> idx_limits(int relative_x) const;
  BoundaryCondition bc() const;
};

class Q1DOpen : public Q1DBase {
public:
  Q1DOpen(SizeParity sp, int dimension_, std::string name_, int nparts_);

private:
  std::tuple<int, int> idx_limits(int relative_x) const;
  BoundaryCondition bc() const;
};

} // namespace slow
} // namespace hypercubes

#endif // Q_H_
