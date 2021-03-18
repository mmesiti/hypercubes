#ifndef __INDEXER_H_
#define __INDEXER_H_
#include "hbb.hpp"
#include "leaf.hpp"
#include "q.hpp"
#include "qr.hpp"

namespace hypercubes {
namespace slow {
namespace partitioning {

struct Indexer {

  int i;
  Indexer() : i(0) {}

  template <int level>
  using Childp = typename partitioning1D::BasePart<level + 1>::P;

  template <int level> auto leaf(int size) {
    return partitioning1D::leaf<level>(i++, size);
  }

  template <int level>
  auto q(int size, //
         Childp<level> quotient) {
    return partitioning1D::q<level>(i++, size, quotient);
  }

  template <int level>
  auto qr(int size,               //
          Childp<level> quotient, //
          Childp<level> rest) {
    return partitioning1D::qr<level>(i++, size, quotient, rest);
  }

  template <int level>
  auto hbb(int size, int halo, //
           Childp<level> bulk, //
           Childp<level> border) {
    return partitioning1D::hbb<level>(i++, size, halo, bulk, border);
  }

}; // namespace I
} // namespace partitioning
} // namespace slow
} // namespace hypercubes
#endif // __INDEXER_H_
