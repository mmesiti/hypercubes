#ifndef PARTITIONERS_H_
#define PARTITIONERS_H_

#include "1D/hbb1D.hpp"
#include "1D/plain1D.hpp"
#include "1D/q1D.hpp"
#include "dimensionalise.hpp"
#include "eo_partitioning.hpp"
#include "partitioning.hpp"
#include <string>
namespace hypercubes {
namespace slow {
namespace partitioners {

class IPartitioningRequest {
public:
  virtual IPartitioning operator()(SizeParitiesD sp) = 0;
};

class EO : public IPartitioningRequest {
public:
  EO(std::string name, hypercubes::slow::EO::CBFlags cbflags);
  hypercubes::slow::EO operator()(SizeParities sp);
};

class QPeriodic : public IPartitioningRequest {
public:
  QPeriodic(std::string name, int dimension, int nparts);
  Dimensionalise<Q1DPeriodic> operator()(SizeParities sp);
};

class QOpen : public IPartitioningRequest {
public:
  QOpen(std::string name, int dimension, int nparts);
  Dimensionalise<Q1DOpen> operator()(SizeParities sp);
};

class Plain : public IPartitioningRequest {
public:
  Plain(std::string name, int dimension);
  Dimensionalise<Plain1D> operator()(SizeParities sp);
};

class HBB : public IPartitioningRequest {
public:
  HBB(std::string name, int dimension, int halo);
  Dimensionalise<HBB1D> operator()(SizeParities sp);
};

} // namespace partitioners
} // namespace slow
} // namespace hypercubes

#endif // PARTITIONERS_H_
