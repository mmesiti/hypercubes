#ifndef PARTITIONERS_H_
#define PARTITIONERS_H_

#include "1D/hbb1D.hpp"
#include "1D/plain1D.hpp"
#include "1D/q1D.hpp"
#include "dimensionalise.hpp"
#include "eo_partitioning.hpp"
#include "partitioning.hpp"
#include "site.hpp"
#include <memory>
#include <string>
namespace hypercubes {
namespace slow {
namespace partitioners {

using partitioning::IPartitioning;

class IPartitioner {
public:
  std::shared_ptr<IPartitioning> partition(SizeParityD sp) const;

  IPartitioner(std::string name_);
  std::string get_name() const;

protected:
  const std::string name;
  virtual IPartitioning *get(SizeParityD sp) const = 0;
};

using CBFlags = hypercubes::slow::partitioning::EO::CBFlags;
class EO : public IPartitioner {
public:
  EO(std::string name, CBFlags cbflags);

private:
  CBFlags cbflags;
  hypercubes::slow::partitioning::EO *get(SizeParityD sp) const;
};

class Site : public IPartitioner {
public:
  Site();

private:
  hypercubes::slow::partitioning::Site *get(SizeParityD sp) const;
};
class QPeriodic : public IPartitioner {
public:
  QPeriodic(std::string name, int dimension, int nparts);

private:
  int dimension, nparts;
  partitioning::Dimensionalise<partitioning::Q1DPeriodic> *
  get(SizeParityD sp) const;
};

class QOpen : public IPartitioner {
public:
  QOpen(std::string name, int dimension, int nparts);

private:
  int dimension, nparts;
  partitioning::Dimensionalise<partitioning::Q1DOpen> *
  get(SizeParityD sp) const;
};

class Plain : public IPartitioner {
public:
  Plain(std::string name, int dimension);

private:
  int dimension;
  partitioning::Dimensionalise<partitioning::Plain1D> *
  get(SizeParityD sp) const;
};

class HBB : public IPartitioner {
public:
  HBB(std::string name, int dimension, int halo);
  int get_dimension() const;

private:
  int dimension, halo;
  partitioning::Dimensionalise<partitioning::HBB1D> *get(SizeParityD sp) const;
};

} // namespace partitioners

using IPartRP = std::shared_ptr<partitioners::IPartitioner>;
using PartList = std::vector<IPartRP>;
namespace partitioner_makers {

using CBFlags = hypercubes::slow::partitioning::EO::CBFlags;
IPartRP EO(std::string name, CBFlags cbflags);
IPartRP Site();
IPartRP QPeriodic(std::string name, int dimension, int nparts);
IPartRP QOpen(std::string name, int dimension, int nparts);
IPartRP Plain(std::string name, int dimension);
IPartRP HBB(std::string name, int dimension, int halo);

} // namespace partitioner_makers
} // namespace slow
} // namespace hypercubes

#endif // PARTITIONERS_H_
