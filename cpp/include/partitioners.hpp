#ifndef PARTITIONERS_H_
#define PARTITIONERS_H_

#include "1D/hbb1D.hpp"
#include "1D/plain1D.hpp"
#include "1D/q1D.hpp"
#include "dimensionalise.hpp"
#include "eo_partitioning.hpp"
#include "partitioning.hpp"
#include <memory>
#include <string>
namespace hypercubes {
namespace slow {
namespace partitioners {

class IPartitioningRequest {
public:
  std::shared_ptr<IPartitioning> partition(SizeParityD sp);

private:
  virtual IPartitioning *get(SizeParityD sp) = 0;
};

using CBFlags = hypercubes::slow::EO::CBFlags;
class EO_ : public IPartitioningRequest {
public:
  EO_(std::string name, CBFlags cbflags);

private:
  hypercubes::slow::EO *get(SizeParityD sp);
  std::string name;
  CBFlags cbflags;
};

class QPeriodic_ : public IPartitioningRequest {
public:
  QPeriodic_(std::string name, int dimension, int nparts);

private:
  std::string name;
  int dimension, nparts;
  Dimensionalise<Q1DPeriodic> *get(SizeParityD sp);
};

class QOpen_ : public IPartitioningRequest {
public:
  QOpen_(std::string name, int dimension, int nparts);

private:
  std::string name;
  int dimension, nparts;
  Dimensionalise<Q1DOpen> *get(SizeParityD sp);
};

class Plain_ : public IPartitioningRequest {
public:
  Plain_(std::string name, int dimension);

private:
  std::string name;
  int dimension;
  Dimensionalise<Plain1D> *get(SizeParityD sp);
};

class HBB_ : public IPartitioningRequest {
public:
  HBB_(std::string name, int dimension, int halo);

private:
  std::string name;
  int dimension, halo;
  Dimensionalise<HBB1D> *get(SizeParityD sp);
};

using IPartRP = std::shared_ptr<IPartitioningRequest>;

IPartRP EO(std::string name, CBFlags cbflags);
IPartRP QPeriodic(std::string name, int dimension, int nparts);
IPartRP QOpen(std::string name, int dimension, int nparts);
IPartRP Plain(std::string name, int dimension);
IPartRP HBB(std::string name, int dimension, int halo);

using PartList = std::vector<IPartRP>;
} // namespace partitioners
} // namespace slow
} // namespace hypercubes

#endif // PARTITIONERS_H_
