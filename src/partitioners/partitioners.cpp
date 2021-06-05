#include "partitioners/partitioners.hpp"
#include <memory>
namespace hypercubes {
namespace slow {
namespace partitioners {
using partitioning::Dimensionalise;

// Constructors
IPartitioner::IPartitioner(std::string name_) : name(name_) {}
EO::EO(std::string name_, hypercubes::slow::partitioning::EO::CBFlags cbflags_)
    : IPartitioner(name_), cbflags(cbflags_) {}
Site::Site() : IPartitioner("Site") {}
QPeriodic::QPeriodic(std::string name_, int dimension_, int nparts_)
    : IPartitioner(name_), dimension(dimension_), nparts(nparts_) {}
QOpen::QOpen(std::string name_, int dimension_, int nparts_)
    : IPartitioner(name_), dimension(dimension_), nparts(nparts_) {}
Plain::Plain(std::string name_, int dimension_)
    : IPartitioner(name_), dimension(dimension_) {}
HBB::HBB(std::string name_, int dimension_, int halo_)
    : IPartitioner(name_), dimension(dimension_), halo(halo_) {}

int HBB::get_dimension() const { return dimension; }

std::shared_ptr<IPartitioning> IPartitioner::partition(SizeParityD sp) const {
  return std::shared_ptr<IPartitioning>(get(sp));
}

std::string IPartitioner::get_name() const { return name; }

class partitioning::EO *EO::get(SizeParities sp) const {
  return new partitioning::EO(sp, cbflags, name);
}

class partitioning::Site *Site::get(SizeParities sp) const {
  return new partitioning::Site(sp);
}

Dimensionalise<partitioning::Q1DPeriodic> *
QPeriodic::get(SizeParityD sp) const {
  return new Dimensionalise<partitioning::Q1DPeriodic>(sp, dimension, name,
                                                       nparts);
}
Dimensionalise<partitioning::Q1DOpen> *QOpen::get(SizeParityD sp) const {
  return new Dimensionalise<partitioning::Q1DOpen>(sp, dimension, name, nparts);
}
Dimensionalise<partitioning::Plain1D> *Plain::get(SizeParityD sp) const {
  return new Dimensionalise<partitioning::Plain1D>(sp, dimension, name);
}
Dimensionalise<partitioning::HBB1D> *HBB::get(SizeParityD sp) const {
  return new Dimensionalise<partitioning::HBB1D>(sp, dimension, name, halo);
}

} // namespace partitioners

namespace partitioner_makers {

using CBFlags = hypercubes::slow::partitioning::EO::CBFlags;
IPartRP EO(std::string name, CBFlags cbflags) {
  return std::make_shared<partitioners::EO>(name, cbflags);
};
IPartRP Site() { //
  return std::make_shared<partitioners::Site>();
};
IPartRP QPeriodic(std::string name, int dimension, int nparts) {
  return std::make_shared<partitioners::QPeriodic>(name, dimension, nparts);
};
IPartRP QOpen(std::string name, int dimension, int nparts) {
  return std::make_shared<partitioners::QOpen>(name, dimension, nparts);
};
IPartRP Plain(std::string name, int dimension) {
  return std::make_shared<partitioners::Plain>(name, dimension);
};
IPartRP HBB(std::string name, int dimension, int halo) {
  return std::make_shared<partitioners::HBB>(name, dimension, halo);
};
} // namespace partitioner_makers
} // namespace slow
} // namespace hypercubes
