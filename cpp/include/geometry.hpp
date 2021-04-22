#ifndef GEOMETRY_H_
#define GEOMETRY_H_
#include <ostream>

namespace hypercubes {
namespace slow {

struct IndexResult {
  int idx;
  int rest;
  int cached_flag;
};

enum Parity { EVEN, ODD, NONE };

struct SizeParity {
  int size;
  Parity parity;
    bool operator==(const SizeParity& other){
        return other.size == size and other.parity == parity;
    }
};

}}
namespace std {
  using SizeParity = hypercubes::slow::SizeParity;
  std::ostream& operator<<(std::ostream& os,SizeParity p);
  bool operator<(SizeParity p1, SizeParity p2);
}
#endif // GEOMETRY_H_
