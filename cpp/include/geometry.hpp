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

using Coordinates = vector<int>;
using Sizes = vector<int>;
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
