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

namespace hypercubes {
namespace slow {
namespace partitioning1D {

/**
 * A hyerarchical partition of 1D ranges
 * supporting unequal parts.
 * Using the composite design pattern.
 * */
template <int level> struct Partition1D {
  int size;
  Partition1D(int size_) : size(size_){};
  /**
   * Given a location along the axis,
   * tell to which partitions it belongs
   * down the tree.
   * */
  virtual std::vector<int> indices(int i) = 0;
  virtual std::ostream &stream(std::ostream &os) const {
    os << size;
    return os;
  }
};

template <int level>
std::ostream &operator<<(std::ostream &os,
                         const Partition1D<level> &partition) {
  return partition.stream(os);
}

template <int level> using Partition1Dp = std::shared_ptr<Partition1D<level>>;

template <int level> struct Partition1DLeaf : Partition1D<level> {
  Partition1DLeaf(int size) : Partition1D<level>(size){};
  std::vector<int> indices(int i) {
    assert(i < Partition1D<level>::size);
    return std::vector<int>({i});
  }
};

template <int level> struct Partition1DComposite : Partition1D<level> {
  Partition1Dp<level - 1> quotient;
  Partition1Dp<level - 1> rest;
  int _nparts;

  Partition1DComposite(int size_, Partition1Dp<level - 1> quotient_,
                       Partition1Dp<level - 1> rest_)
      : Partition1D<level>(size_), quotient(quotient_), rest(rest_) {
    assert(Partition1D<level>::size % quotient->size == rest->size);
    _nparts = size_ / quotient->size + ((rest->size == 0) ? 0 : 1);
  }

  std::vector<int> indices(int i) {
    assert(i < Partition1D<level>::size);
    int idx = i / quotient->size;
    int rst = i % quotient->size;
    std::vector<int> res{idx};
    auto other_indices = _where(i)->indices(rst);
    res.insert(res.end(), other_indices.begin(), other_indices.end());
    return res;
  }

  Partition1Dp<level - 1> _where(int i) {
    int idx = i / quotient->size;
    if (idx == _nparts - 1 and rest->size != 0)
      return rest;
    else
      return quotient;
  }

  std::ostream &stream(std::ostream &os) const {
    Partition1D<level>::stream(os) << ", {";
    os << *quotient << ", " << *rest << "}";
    return os;
  }
};

/// Function to create a leaf
Partition1Dp<1> p(int size) {
  return std::make_shared<Partition1DLeaf<1>>(size);
}
/// Function to create a composite
template <int level>
Partition1Dp<level> p(int size, Partition1Dp<level - 1> quotient,
                      Partition1Dp<level - 1> rest) {
  return std::make_shared<Partition1DComposite<level>>(size, quotient, rest);
}

} // namespace partitioning1D
} // namespace slow
} // namespace hypercubes

#endif // __PARTITIONS_H_
