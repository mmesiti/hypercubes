#ifndef __QR_H_
#define __QR_H_

#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <iterator>
#include <memory>
#include <vector>

#include "base.hpp"
#include "toposorting.hpp"

namespace hypercubes {
namespace slow {
namespace partitioning1D {
using toposorting::SortablePartitioning;

template <int level> struct QR : BasePart<level> {
  using Base = BasePart<level>;
  using Childp = typename BasePart<level + 1>::P;
  using ChildCoord = Coordinate<level + 1>;
  using Partition = Partition1D<level>;

  Childp quotient;
  Childp rest;

  int _nparts;

  QR(int id,
     int size,         //
     Childp quotient_, //
     Childp rest_)
      : Base(id, size),      //
        quotient(quotient_), //
        rest(rest_) {
    assert(Base::end() % quotient->size == rest->end());
    _nparts = Base::end() / quotient->size + ((rest->not_empty()) ? 1 : 0);
  }

  std::vector<int> indices(ChildCoord x) {
    assert(x < Base::end());
    int idx = x / quotient->size;
    auto rst = x % quotient->size;
    std::vector<int> res{idx};
    auto other_indices = _where(x)->indices(rst);
    res.insert(res.end(), other_indices.begin(), other_indices.end());
    return res;
  }

  Childp _where(ChildCoord x) {
    int idx = x / quotient->size;
    if (idx == _nparts - 1 and rest->not_empty())
      return rest;
    else
      return quotient;
  }

  std::ostream &stream(std::ostream &os) const {
    return Base::stream(os) << ", {"                       //
                            << "q:{" << *quotient << "}, " //
                            << "r:{" << *rest << "}}";
  }

  Partition operator[](int i) {
    return Partition{quotient->size * i, //
                     std::min(quotient->size * (i + 1), Base::end())};
  }

  int max_subtree_level() {
    return 1 +
           std::max(quotient->max_subtree_level(), rest->max_subtree_level());
  }

  bool is_balanced() {
    return quotient->is_balanced() and rest->is_balanced() and
           quotient->max_subtree_level() == rest->max_subtree_level();
  };
  std::vector<SortablePartitioning::P> children() const {
    return std::vector<SortablePartitioning::P>{quotient, rest};
  }
};
template <int level> using QRp = std::shared_ptr<QR<level>>;
template <int level>
QRp<level> qr(int id, int size,                         //
              typename BasePart<level + 1>::P quotient, //
              typename BasePart<level + 1>::P rest) {
  return std::make_shared<QR<level>>(id, size, quotient, rest);
}

} // namespace partitioning1D
} // namespace slow
} // namespace hypercubes

#endif // __PARTITIONS_H_
