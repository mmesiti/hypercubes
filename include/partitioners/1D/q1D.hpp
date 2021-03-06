#ifndef Q_H_
#define Q_H_
#include "partitioners/dimensionalise.hpp"
#include "partitioning1D.hpp"
#include <tuple>

namespace hypercubes {
namespace slow {
namespace partitioning {

class Q1DBase : public Partitioning1D {
public:
  Q1DBase(PartInfo sp, int dimension_, std::string name_, int nparts_);
  int idx_to_coord(int idx, int offset) const;
  int max_idx_value() const;
  std::string comments() const;
  std::vector<IndexResult> coord_to_idxs(int relative_x) const;

protected:
  int nparts;
  int quotient;

private:
  auto key() const {
    return std::make_tuple(size, parity, dimension, name, nparts, bc());
  }
  virtual std::tuple<int, int> idx_limits(int relative_x) const = 0;
  virtual BoundaryCondition bc() const = 0;
};

class Q1DPeriodic : public Q1DBase {
public:
  Q1DPeriodic(PartInfo sp, int dimension_, std::string name_, int nparts_);

private:
  std::tuple<int, int> idx_limits(int relative_x) const;
  BoundaryCondition bc() const;
};

class Q1DOpen : public Q1DBase {
public:
  Q1DOpen(PartInfo sp, int dimension_, std::string name_, int nparts_);

private:
  std::tuple<int, int> idx_limits(int relative_x) const;
  BoundaryCondition bc() const;
};
} // namespace partitioning
} // namespace slow
} // namespace hypercubes

namespace std {
ostream &operator<<(ostream &os, hypercubes::slow::BoundaryCondition bc);
}

#endif // Q_H_
