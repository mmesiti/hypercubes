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
struct Partition1D {
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
  virtual int max_level() = 0;
  virtual bool tree_is_balanced() = 0;
};

std::ostream &operator<<(std::ostream &os, const Partition1D &partition) {
  return partition.stream(os);
}

using Partition1Dp = std::shared_ptr<Partition1D>;

struct Partition1DLeaf : Partition1D {
  Partition1DLeaf(int size) : Partition1D(size){};
  std::vector<int> indices(int i) {
    assert(i < size);
    return std::vector<int>({i});
  }
  int max_level() { return 1; }
  bool tree_is_balanced() { return true; }
  virtual Piece1D operator[](int i) = 0;
};

struct Partition1DComposite : Partition1D {
  Partition1Dp quotient;
  Partition1Dp rest;
  int _nparts;

  Partition1DComposite(int size_, Partition1Dp quotient_, Partition1Dp rest_)
      : Partition1D(size_), quotient(quotient_), rest(rest_) {
    assert(size % quotient->size == rest->size);
    _nparts = size_ / quotient->size + ((rest->size == 0) ? 0 : 1);
  }

  std::vector<int> indices(int i) {
    assert(i < size);
    int idx = i / quotient->size;
    int rst = i % quotient->size;
    std::vector<int> res{idx};
    auto other_indices = _where(i)->indices(rst);
    res.insert(res.end(), other_indices.begin(), other_indices.end());
    return res;
  }

  Partition1Dp _where(int i) {
    int idx = i / quotient->size;
    if (idx == _nparts - 1 and rest->size != 0)
      return rest;
    else
      return quotient;
  }

  int max_level() {
    int qnlevel = quotient->max_level();
    int rnlevel = rest->max_level();
    return std::max(qnlevel, rnlevel) + 1;
  }

  bool tree_is_balanced() {
    int qnlevel = quotient->max_level();
    int rnlevel = rest->max_level();
    return qnlevel == rnlevel               // levels are the same
           and quotient->tree_is_balanced() // both subtrees must be balanced
           and rest->tree_is_balanced();
  }

  std::ostream &stream(std::ostream &os) const {
    Partition1D::stream(os) << ", {";
    os << *quotient << ", " << *rest << "}";
    return os;
  }
};

/// Function to create a leaf
Partition1Dp p(int size) { return std::make_shared<Partition1DLeaf>(size); }
/// Function to create a composite
Partition1Dp p(int size, Partition1Dp quotient, Partition1Dp rest) {
  return std::make_shared<Partition1DComposite>(size, quotient, rest);
}

} // namespace partitioning1D
} // namespace slow
} // namespace hypercubes

#endif // __PARTITIONS_H_
