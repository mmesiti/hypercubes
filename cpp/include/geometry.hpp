#ifndef GEOMETRY_H_
#define GEOMETRY_H_
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

struct IndexResultD {
  int idx;
  vector<int> rest;
  bool cached_flag;
};

enum Parity { EVEN, ODD, NONE };

struct SizeParity {
  int size;
  Parity parity;
};

// Coordinates are for locations in physical space - e.g, 4D
using Coordinates = vector<int>;
// Sizes are for extents in physical space
using Sizes = vector<int>;
// Indices are for location in the partitioning tree
// (not that different from coordinates, after all,
// but in a space with a different dimensionality
// and some other constraint)
using Indices = vector<int>;
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
} // namespace slow
} // namespace hypercubes
namespace std {
std::ostream &operator<<(std::ostream &os, hypercubes::slow::Parity p);
std::ostream &operator<<(std::ostream &os, hypercubes::slow::SizeParity p);
} // namespace std
#endif // GEOMETRY_H_
