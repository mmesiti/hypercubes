#include "partitioners.hpp"
#include <memory>
namespace hypercubes {
namespace slow {
namespace partitioners {

// Constructors
EO_::EO_(std::string name_, hypercubes::slow::EO::CBFlags cbflags_)
    : name(name_), cbflags(cbflags_) {}
QPeriodic_::QPeriodic_(std::string name_, int dimension_, int nparts_)
    : name(name_), dimension(dimension_), nparts(nparts_) {}
QOpen_::QOpen_(std::string name_, int dimension_, int nparts_)
    : name(name_), dimension(dimension_), nparts(nparts_) {}
Plain_::Plain_(std::string name_, int dimension_)
    : name(name_), dimension(dimension_) {}
HBB_::HBB_(std::string name_, int dimension_, int halo_)
    : name(name_), dimension(dimension_), halo(halo_) {}

std::shared_ptr<IPartitioning> IPartitioningRequest::partition(SizeParityD sp) {
  return std::shared_ptr<IPartitioning>(get(sp));
}

hypercubes::slow::EO *EO_::get(SizeParities sp) {
  return new hypercubes::slow::EO(sp, cbflags, name);
}

Dimensionalise<Q1DPeriodic> *QPeriodic_::get(SizeParityD sp) {
  return new Dimensionalise<Q1DPeriodic>(sp, dimension, name, nparts);
}
Dimensionalise<Q1DOpen> *QOpen_::get(SizeParityD sp) {
  return new Dimensionalise<Q1DOpen>(sp, dimension, name, nparts);
}
Dimensionalise<Plain1D> *Plain_::get(SizeParityD sp) {
  return new Dimensionalise<Plain1D>(sp, dimension, name);
}
Dimensionalise<HBB1D> *HBB_::get(SizeParityD sp) {
  return new Dimensionalise<HBB1D>(sp, dimension, name, halo);
}

IPartRP EO(std::string name, CBFlags cbflags) {
  return std::make_shared<EO_>(name, cbflags);
};
IPartRP QPeriodic(std::string name, int dimension, int nparts) {
  return std::make_shared<QPeriodic_>(name, dimension, nparts);
};
IPartRP QOpen(std::string name, int dimension, int nparts) {
  return std::make_shared<QOpen_>(name, dimension, nparts);
};
IPartRP Plain(std::string name, int dimension) {
  return std::make_shared<Plain_>(name, dimension);
};
IPartRP HBB(std::string name, int dimension, int halo) {
  return std::make_shared<HBB_>(name, dimension, halo);
};
} // namespace partitioners
} // namespace slow
} // namespace hypercubes
