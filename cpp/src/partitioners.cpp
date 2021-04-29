#include "partitioners.hpp"
namespace hypercubes {
namespace slow {
namespace partitioners {

EO::EO(std::string name, hypercubes::slow::EO::CBFlags cbflags) {}
hypercubes::slow::EO EO::operator()(SizeParities sp) {}

QPeriodic::QPeriodic(std::string name, int dimension, int nparts) {}
Dimensionalise<Q1DPeriodic> QPeriodic::operator()(SizeParities sp) {}

QOpen::QOpen(std::string name, int dimension, int nparts) {}
Dimensionalise<Q1DOpen> QOpen::operator()(SizeParities sp) {}

Plain::Plain(std::string name, int dimension) {}
Dimensionalise<Plain1D> Plain::operator()(SizeParities sp) {}

HBB::HBB(std::string name, int dimension, int halo) {}
Dimensionalise<HBB1D> HBB::operator()(SizeParities sp) {}

} // namespace partitioners
} // namespace slow
} // namespace hypercubes
