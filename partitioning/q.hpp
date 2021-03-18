#ifndef __PARTITIONINGQ_H_
#define __PARTITIONINGQ_H_

#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <iterator>
#include <memory>
#include <vector>

#include "base.hpp"

namespace hypercubes {
namespace slow {
namespace partitioning1D {

template <int level> struct Q : BasePart<level> {
  using Base = BasePart<level>;
  using Childp = typename BasePart<level + 1>::P;
  using ChildCoord = Coordinate<level + 1>;
  using Partition = Partition1D<level>;

  Childp quotient;

  Q(int id, int size, //
    Childp quotient_)
      : Base(id, size),       //
        quotient(quotient_) { //
    assert((Base::end() % quotient->size).is_zero());
  }

  std::vector<int> indices(ChildCoord x) {
    assert(x < Base::end());
    int idx = x / quotient->size;
    auto rst = x % quotient->size;
    std::vector<int> res{idx};
    auto other_indices = quotient->indices(rst);
    res.insert(res.end(), other_indices.begin(), other_indices.end());
    return res;
  }

  std::ostream &stream(std::ostream &os) const {
    Base::stream(os) << ", {q:{" << *quotient << "}}";
    return os;
  }

  Partition operator[](int i) {
    return Partition{quotient->size * i, //
                     quotient->size * (i + 1)};
  }

  int max_subtree_level() { return quotient->max_subtree_level() + 1; }

  bool is_balanced() { return quotient->is_balanced(); };
  std::vector<SortablePartitioning::P> children() const {
    return std::vector<SortablePartitioning::P>{quotient};
  }
};

template <int level> using Qp = std::shared_ptr<Q<level>>;
template <int level>
auto q(int id, int size, //
       typename BasePart<level + 1>::P quotient) {
  return std::make_shared<Q<level>>(id, size, quotient);
}

} // namespace partitioning1D
} // namespace slow
} // namespace hypercubes

#endif // K
