#ifndef __HBB_H_
#define __HBB_H_

#include <cassert>
#include <memory>
#include <ostream>

#include "base.hpp"
#include "toposorting.hpp"

namespace hypercubes {
namespace slow {
namespace partitioning1D {

/**
 * Halo-Border-Bulk partitioning
 * */
template <int level> struct HBB : BasePart<level> {
  using Base = BasePart<level>;
  using Child = BasePart<level + 1>;
  using Childp = typename Child::P;
  using Coord = Coordinate<level>;
  using ChildCoord = Coordinate<level + 1>;
  using Partition = Partition1D<level>;
  using Offset = typename ChildCoord::Offset;

  Childp bulk;
  Childp border; // also for halo
  enum Portion {
    HALO_MINUS,
    BORDER_MINUS,
    BULK,
    BORDER_PLUS,
    HALO_PLUS,
    NUM_PORTIONS
  };
  ChildCoord starts[6];

  HBB(int id,       //
      int size,     //
      int hs,       //
      Childp bulk_, //
      Childp border_)
      : Base(id, size),  //
        bulk(bulk_),     //
        border(border_), //
        starts{
            Base::start() - Offset{hs}, // HALO_MINUS,
            Base::start(),              // BORDER_MINUS,
            Base::start() + Offset{hs}, // BULK,
            Base::end() - Offset{hs},   // BORDER_PLUS,
            Base::end(),                // HALO_PLUS,
            Base::end() + Offset{hs}    // NUM_PORTIONS
        } {
    assert(border->size.value == hs);
    assert(bulk->size.value == Base::size.value - hs * 2);
  };
  auto start(Portion p) { return starts[p]; }
  auto end(Portion p) { return starts[p + 1]; }

  Portion get_portion(ChildCoord x) {
    for (Portion p : {HALO_MINUS, BORDER_MINUS, BULK, BORDER_PLUS, HALO_PLUS}) {
      if (x >= start(p) and x < end(p))
        return p;
    }
    assert(false);
    return NUM_PORTIONS;
  }

  std::vector<int> indices(ChildCoord x) {
    assert(x < Base::end());
    int idx = get_portion(x);
    auto rst = x - starts[idx];
    std::vector<int> res{idx};
    auto other_indices = _where(x)->indices(rst);
    res.insert(res.end(), other_indices.begin(), other_indices.end());
    return res;
  }

  Childp _where(ChildCoord x) {
    int idx = get_portion(x);
    if (idx == BULK)
      return bulk;
    else
      return border;
  };

  std::ostream &stream(std::ostream &os) const {
    return Base::stream(os) << ", {"                      //
                            << "bulk:{" << *bulk << "}, " //
                            << "border:{" << *border << "}}";
  }

  int max_subtree_level() {
    return 1 + std::max(bulk->max_subtree_level(), border->max_subtree_level());
  }

  bool is_balanced() {
    return bulk->is_balanced() and border->is_balanced() and
           bulk->max_subtree_level() == border->max_subtree_level();
  }

  Partition operator[](int i) {
    auto p = (Portion)i;
    return Partition{start(p), end(p)};
  }
  std::vector<SortablePartitioning::P> children() const {
    return std::vector<SortablePartitioning::P>{bulk, border};
  }

  bool operator<(const SortablePartitioning &other) const {
    return toposorting::strict_relationship(*this, other);
  }
};

template <int level> using HBBp = std::shared_ptr<HBB<level>>;
template <int level>
HBBp<level> hbb(int id, int size, int halo,           //
                typename BasePart<level + 1>::P bulk, //
                typename BasePart<level + 1>::P border) {
  using HBB = HBB<level>;

  return std::make_shared<HBB>(id, size, halo, bulk, border);
}

} // namespace partitioning1D
} // namespace slow
} // namespace hypercubes

#endif // __HBB_H_
