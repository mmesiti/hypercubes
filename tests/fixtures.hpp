#ifndef FIXTURES_H_
#define FIXTURES_H_
#include "partitioners/partitioners.hpp"
#include "trees/partition_tree.hpp"

namespace hypercubes {
namespace slow {
namespace internals {

// Partitioning expected, for each dimension, until EO level:
// Site index:
// tens:    |0 0000 0 0 000 1 1 1111 1 1 112 2 2 2222 2 2 233 3 3 333 3 3 34 4|
// units:   |0 1234 5 6 789 0 1 2345 6 7 890 1 2 3456 7 8 901 2 3 456 7 8 90 1|
// Sizes,l0: 11               11               11               9
//          |0 0000 0 0 000 0|1 1111 1 1 111 1|2 2222 2 2 222 2|3 333 3 3 33 3|
// Sizes,l1: 6        5       6        5       6        5       5       4
//          |0 0000 0|1 111 1|0 0000 0|1 111 1|0 0000 0|1 111 1|0 000 0|1 11 1|
// brdr-blk:|1|2222|3|1|222|3|1|2222|3|1|222|3|1|2222|3|1|222|3|1|222|3|1|22|3|
// In-p idx:|0|0123|0|0|012|0|0|0123|0|0|012|0|0|0123|0|0|012|0|0|012|0|0|01|0|
// In-p sz:  1 4    1 1 3   1 1 4    1 1 3   1 1 4    1 1 3   1 1 3   1 1 2  1
std::vector<int> get_bulk_sites42();
std::vector<int> get_border_sites42();
std::vector<int> _assemble(const std::string &tens, const std::string &units);
struct Part4DF {

  enum { X, Y, Z, T, EXTRA };
  SizeParityD sp;
  PartList partitioners;
  PartitionTree t;
  std::vector<int> bulk_sites;
  std::vector<int> border_sites;

  Part4DF();
};

struct Part4DFWLocal {

  enum { X, Y, Z, T, LOCAL, EXTRA };
  SizeParityD sp;
  PartList partitioners;
  PartitionTree t;
  std::vector<int> bulk_sites;
  std::vector<int> border_sites;

  Part4DFWLocal();
};

struct Part1D42 {

  enum { X, EXTRA };
  SizeParityD sp;
  PartList partitioners;
  PartitionTree t;
  std::vector<int> bulk_sites;
  std::vector<int> border_sites;
  vector<std::pair<int, Indices>> haloonly1D;
  vector<std::pair<int, Indices>> get_haloonly1D();

  Part1D42();
};

} // namespace internals
} // namespace slow
} // namespace hypercubes

#endif // FIXTURES_H_
