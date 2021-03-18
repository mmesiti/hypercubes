#ifndef __PARTITIONS_H_
#define __PARTITIONS_H_

#include <algorithm>
#include <array>
#include <cassert>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <memory>
#include <vector>

#include "base.hpp"

namespace hypercubes {
namespace slow {
namespace partitioning1D {

#define _TYPE_DEFINITIONS                                                      \
  using Base = BasePart<level>;                                                \
  using Child = BasePart<level + 1>;                                           \
  using Childp = BaseP<level + 1>;                                             \
  using Coord = Coordinate<level>;                                             \
  using ChildCoord = Coordinate<level + 1>;                                    \
  using Partition = Partition1D<level>;

template <int level> struct Q : BasePart<level> {

  _TYPE_DEFINITIONS

  Childp quotient;

  Q(int size, //
    Childp quotient_)
      : Base(size),           //
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
};

template <int level> struct QR : BasePart<level> {

  _TYPE_DEFINITIONS

  Childp quotient;
  Childp rest;

  int _nparts;

  QR(int size,         //
     Childp quotient_, //
     Childp rest_)
      : Base(size),          //
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
};
template <int level>
auto qr(int size, BaseP<level + 1> quotient, BaseP<level + 1> rest) {
  return std::make_shared<QR<level>>(size, quotient, rest);
}

template <int level> auto q(int size, BaseP<level + 1> quotient) {

  return std::make_shared<Q<level>>(size, quotient);
}

} // namespace partitioning1D
} // namespace slow
} // namespace hypercubes

#endif // __PARTITIONS_H_
