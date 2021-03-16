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
 * Templatized coordinate type
 * not to confuse different levels. */
template <int level> struct Coord {
  int x;
  Coord(int x_) : x(x_){};
};

/**
 * Portion representing a piece in the partitioning. */
template <int level> struct Partition1D {
  Coord<level> start;
  Coord<level> end;
};

/**
 * A hyerarchical partitioning of 1D ranges
 * supporting unequal parts.
 * Using the composite design pattern.
 * */
template <int level> struct Partitioning1D {
  int size;
  Partitioning1D(int size_) : size(size_){};
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
  virtual Partition1D<level> operator[](int i) = 0;
  virtual bool is_balanced() = 0;
  virtual int max_subtree_level() = 0;
};

template <int level>
std::ostream &operator<<(std::ostream &os,
                         const Partitioning1D<level> &partition) {
  return partition.stream(os);
}

template <int level>
using Partitioning1Dp = std::shared_ptr<Partitioning1D<level>>;

template <int level> struct Partitioning1DLeaf : Partitioning1D<level> {
  Partitioning1DLeaf(int size) : Partitioning1D<level>(size){};
  std::vector<int> indices(int i) {
    assert(i < Partitioning1D<level>::size);
    return std::vector<int>({i});
  }
  Partition1D<level> operator[](int i) { return Partition1D<level>{i, i + 1}; }

  bool is_balanced() { return true; };
  int max_subtree_level() { return level; };
};

template <int level> struct Partitioning1DComposite : Partitioning1D<level> {
  Partitioning1Dp<level + 1> quotient;
  Partitioning1Dp<level + 1> rest;
  int _nparts;

  Partitioning1DComposite(int size_, Partitioning1Dp<level + 1> quotient_,
                          Partitioning1Dp<level + 1> rest_)
      : Partitioning1D<level>(size_), quotient(quotient_), rest(rest_) {
    assert(Partitioning1D<level>::size % quotient->size == rest->size);
    _nparts = size_ / quotient->size + ((rest->size == 0) ? 0 : 1);
  }

  std::vector<int> indices(int i) {
    assert(i < Partitioning1D<level>::size);
    int idx = i / quotient->size;
    int rst = i % quotient->size;
    std::vector<int> res{idx};
    auto other_indices = _where(i)->indices(rst);
    res.insert(res.end(), other_indices.begin(), other_indices.end());
    return res;
  }

  Partitioning1Dp<level + 1> _where(int i) {
    int idx = i / quotient->size;
    if (idx == _nparts - 1 and rest->size != 0)
      return rest;
    else
      return quotient;
  }

  std::ostream &stream(std::ostream &os) const {
    Partitioning1D<level>::stream(os) << ", {";
    os << *quotient << ", " << *rest << "}";
    return os;
  }

  Partition1D<level> operator[](int i) {
    return Partition1D<level>(
        {i * quotient->size,
         std::min((i + 1) * quotient->size, Partitioning1D<level>::size)});
  }

  int max_subtree_level() {
    return std::max(quotient->max_subtree_level(), rest->max_subtree_level());
  }

  bool is_balanced() {
    return quotient->is_balanced() and rest->is_balanced() and
           quotient->max_subtree_level() == rest->max_subtree_level();
  };
};

// only to declare p(int) as a template for uniformity
/// Function to create a leaf
template <int level> Partitioning1Dp<level> p(int size) {
  return std::make_shared<Partitioning1DLeaf<level>>(size);
}
/// Function to create a composite
template <int level>
Partitioning1Dp<level> p(int size, Partitioning1Dp<level + 1> quotient,
                         Partitioning1Dp<level + 1> rest) {
  return std::make_shared<Partitioning1DComposite<level>>(size, quotient, rest);
}

} // namespace partitioning1D
} // namespace slow
} // namespace hypercubes

#endif // __PARTITIONS_H_
