#ifndef GEOMETRY_H_
#define GEOMETRY_H_
#include "int_vec_wrappers.hpp"
#include <ostream>
#include <vector>

namespace hypercubes {
namespace slow {
using std::vector;

struct IndexResult {
  int idx;
  int rest;
  bool cached_flag;
};

enum Parity { EVEN, ODD, NONE };
enum BoundaryCondition { PERIODIC, OPEN };

struct SizeParity {
  int size;
  Parity parity;
};

// Sizes are for extents in physical space
using Sizes = vector<int>;

struct IndexResultD {
  int idx;
  Coordinates rest;
  bool cached_flag;
};

// [nclasses]
using SizeParities = vector<SizeParity>;
// [dimensions]
using SizeParityD = vector<SizeParity>;
// note:
// conceptually, SizeParitiesD != vector<SizeParities>:
// [nclasses][dimensions]
using SizeParitiesD = vector<vector<SizeParity>>;

bool operator<(SizeParity p1, SizeParity p2);
bool operator==(SizeParity p1, SizeParity p2);
bool operator==(IndexResult i1, IndexResult i2);

Coordinates up(Coordinates coords, //
               int dir);
Coordinates down(Coordinates coords, //
                 int dir);
Coordinates up(Coordinates coords,            //
               Sizes sizes,                   //
               vector<BoundaryCondition> bcs, //
               int dir);
Coordinates down(Coordinates coords,            //
                 Sizes sizes,                   //
                 vector<BoundaryCondition> bds, //
                 int dir);

} // namespace slow
} // namespace hypercubes
namespace std {
std::ostream &operator<<(std::ostream &os, hypercubes::slow::Parity p);
std::ostream &operator<<(std::ostream &os, hypercubes::slow::SizeParity sp);
std::ostream &operator<<(std::ostream &os,
                         const hypercubes::slow::Coordinates &cs);
std::ostream &operator<<(std::ostream &os,
                         const hypercubes::slow::Indices &idxs);
} // namespace std
#endif // GEOMETRY_H_
