#ifndef MEMORY_LAYOUT_INTTYPES_H_
#define MEMORY_LAYOUT_INTTYPES_H_
#include "memory_layout.hpp"

namespace hypercubes::slow {
template <int N> class PartitionTreeN;
template <int N> class NChildrenTreeN;
template <int N> class SizeTreeN;
template <int N> class OffsetTreeN;

template <int N> class IndicesN : public Indices {
public:
  IndicesN(Indices &&idxs) : Indices(idxs){};
};
template <int N> class OffsetTreeN : public OffsetTree {
public:
  OffsetTreeN(const SizeTreeN<N> &st) : OffsetTree(st){};

  // TODO: the non N-specific methods should be made protected
  //       in the base class, not public
  int get_offsetN(const IndicesN<N> &idxs) { return get_offset(idxs); }
  template <int M> OffsetTreeN<M> get_subtree(const IndicesN<N> &idxs) {
    return get_subtree(idxs);
  }
  IndicesN<N> get_indicesN(int offset) const {
    return IndicesN<N>(OffsetTree::get_indices(offset));
  }
};
template <int N> class SizeTreeN : public SizeTree {
public:
  SizeTreeN(const NChildrenTreeN<N> &nct) : SizeTree(nct){};
  // TODO: the non N-specific methods should be made protected
  //       in the base class, not public
  int get_size(const IndicesN<N> &idxs) const {
    return SizeTree::get_size(idxs);
  }
  template <int M> SizeTreeN<M> get_subtree(const IndicesN<N> &idxs) const;
  OffsetTreeN<N> offset_tree() const {
    return static_cast<OffsetTreeN<N> &&>(SizeTree::offset_tree());
  }
  friend OffsetTreeN<N>::OffsetTreeN(const SizeTreeN<N> &);
};
template <int N> class NChildrenTreeN : public NChildrenTree {
public:
  NChildrenTreeN(const PartitionTreeN<N> &pt) : NChildrenTree(pt){};

  template <int M>
  NChildrenTreeN<M>
  permute(const vector<std::string> &_permuted_level_names) const {

    return static_cast<NChildrenTreeN<M> &&>(
        NChildrenTree::permute(_permuted_level_names));
  }
  NChildrenTreeN<N> prune(const PartitionPredicate &p) const {
    return static_cast<NChildrenTreeN<N> &&>(NChildrenTree::prune(p));
  }
  int get_nchildren(const IndicesN<N> &) const;
  template <int M> NChildrenTreeN<M> get_subtree(const IndicesN<N> &idxs) const;
  SizeTreeN<N> size_tree() const {
    return static_cast<SizeTreeN<N> &&>(NChildrenTree::size_tree());
  }
  friend SizeTreeN<N>::SizeTreeN(const NChildrenTreeN<N> &);
};

template <int N> class PartitionTreeN : public PartitionTree {
public:
  template <class... Args>
  PartitionTreeN(Args... args) : PartitionTree(args...){};

  IndicesN<N> get_indices(const Coordinates &xs) const {
    return IndicesN<N>(PartitionTree::get_indices(xs));
  }
  vector<std::pair<int, IndicesN<N>>>
  get_indices_wgN(const Coordinates &xs) const {
    auto res_base = PartitionTree::get_indices_wg(xs);
    return vtransform(res_base, [](auto f_idxs) {
      return std::make_pair(f_idxs.first, //
                            IndicesN<N>(std::move(f_idxs.second)));
    });
  }
  Coordinates get_coordinatesN(const IndicesN<N> &idxs) const {
    return PartitionTree::get_coordinates(idxs);
  }
  friend NChildrenTreeN<N>::NChildrenTreeN(const PartitionTreeN<N> &pt);
  NChildrenTreeN<N> nchildren_tree() const { return NChildrenTreeN<N>(*this); };
};

} // namespace hypercubes::slow

#endif // MEMORY_LAYOUT_INTTYPES_H_
