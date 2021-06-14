#ifndef PARTITION_TREE_H_
#define PARTITION_TREE_H_
#include "geometry/geometry.hpp"
#include "selectors/partition_predicates.hpp"
#include "selectors/prune_tree.hpp"
#include "trees/kvtree.hpp"
#include "trees/partition_tree.hpp"
#include "trees/partition_tree_allocations.hpp"

#include "trees/level_swap.hpp" // TODO: move elsewhere
#include <iostream>             //DEBUG
namespace hypercubes {
namespace slow {
template <int N> class IndicesN : public Indices {
public:
  IndicesN(Indices &&idxs) : Indices(idxs){};
};
template <int N> class PartitionTree;
template <int N> class NChildrenTree;
template <int N> class SizeTree;
template <int N> class OffsetTree;

template <int N> class OffsetTree {
private:
  vector<std::string> level_names;
  internals::KVTreeP<int> offset_tree;

  OffsetTree(internals::KVTreeP<int> &&st, vector<std::string> &&ln)
      : offset_tree(st), level_names(ln) {}

public:
  const internals::KVTreeP<int> get_internal() const { return offset_tree; }

  template <int M>
  friend OffsetTree<M>(internals::KVTreeP<int> &&st, vector<std::string> &&ln);

  OffsetTree(const SizeTree<N> &size_tree)
      : offset_tree(get_offset_tree(size_tree.size_tree)),
        level_names(size_tree.level_names) {}
  vector<std::string> get_level_names() const { return level_names; }

  template <int M> OffsetTree<M> get_subtree(const IndicesN<N> &idxs) const {
    try {
      auto subtree = internals::select_subtree_kv(offset_tree, idxs);
      return OffsetTree<M>(std::move(subtree), tail(level_names, idxs.size()));
    } catch (internals::KeyNotFoundError &e) {
      std::cout << "Indices invalid: " << std::endl;
      std::cout << idxs << std::endl;
      std::cout << level_names << std::endl;
      throw std::invalid_argument(e.what());
    }
  }

  int get_offset(const IndicesN<N> &idxs) const {
    try {
      return get_subtree(idxs).offset_tree->n.second;
    } catch (internals::KeyNotFoundError &e) {
      std::cout << "Indices invalid: " << std::endl;
      std::cout << idxs << std::endl;
      std::cout << level_names << std::endl;
      throw std::invalid_argument(e.what());
    }
  }

  OffsetTree shift(int shift) const {
    auto shifted_tree = internals::shift_tree(offset_tree, shift);
    auto ln = level_names;
    return OffsetTree(std::move(shifted_tree), std::move(ln));
  }
  IndicesN<N> get_indices(int offset) const {
    return internals::search_in_sorted_tree(offset_tree, offset);
  }
};

template <int N> class SizeTree {
private:
  vector<std::string> level_names;
  internals::KVTreeP<int> size_tree;

  SizeTree(internals::KVTreeP<int> &&st, vector<std::string> &&ln)
      : size_tree(st), level_names(ln) {}

public:
  vector<std::string> get_level_names() const { return level_names; }
  SizeTree(const NChildrenTree<N> &nchildren_tree)
      : size_tree(get_size_tree(nchildren_tree.nchildren_tree)),
        level_names(nchildren_tree.level_names) {}
  const internals::KVTreeP<int> get_internal() const { return size_tree; }
  template <int M> SizeTree<M> get_subtree(const IndicesN<N> &idxs) const {
    try {
      auto subtree = internals::select_subtree_kv(size_tree, idxs);
      return SizeTree<M>(std::move(subtree), tail(level_names, idxs.size()));
    } catch (internals::KeyNotFoundError &e) {
      std::cout << "Indices invalid: " << std::endl;
      std::cout << idxs << std::endl;
      std::cout << level_names << std::endl;
      throw std::invalid_argument(e.what());
    }
  }
  int get_size(const IndicesN<N> &idxs) const {
    try {
      return get_subtree(idxs).size_tree->n.second;
    } catch (internals::KeyNotFoundError &e) {
      std::cout << "Indices invalid: " << std::endl;
      std::cout << idxs << std::endl;
      std::cout << level_names << std::endl;
      throw std::invalid_argument(e.what());
    }
  }
  friend OffsetTree<N>::OffsetTree(const SizeTree<N> &size_tree);
  OffsetTree<N> offset_tree() const { return OffsetTree<N>(*this); }
};

template <int N> class NChildrenTree {
private:
  vector<std::string> level_names;
  internals::KVTreeP<int> nchildren_tree;

  NChildrenTree(internals::KVTreeP<int> &&_nct, vector<std::string> &&_ln)
      : nchildren_tree(_nct), level_names(_ln) {}

public:
  vector<std::string> get_level_names() const { return level_names; }

  NChildrenTree(const PartitionTree<N> &pt)
      : nchildren_tree(get_nchildren_alloc_tree(pt.partition_tree)),
        level_names(internals::get_partitioners_names(pt.partitioners_list)) //
  {}

  NChildrenTree<N> prune(const PartitionPredicate &predicate) const {
    auto pruned_tree = internals::prune_tree(nchildren_tree, predicate);
    auto pruned_level_names = level_names;
    return NChildrenTree(std::move(pruned_tree), std::move(pruned_level_names));
  }
  template <int M>
  NChildrenTree<M>
  permute(const vector<std::string> &permuted_level_names) const {
    try {

      auto permuted_tree = internals::swap_levels(
          nchildren_tree, //
          internals::find_permutation(permuted_level_names, level_names));
      vector<std::string> _pmlcp = permuted_level_names;
      return NChildrenTree<M>(std::move(permuted_tree), std::move(_pmlcp));
    } catch (const internals::KeyNotFoundError &e) {
      std::cout << "Error in swapping levels in the tree:" << std::endl;
      std::cout << level_names << std::endl;
      std::cout << "->" << std::endl;
      std::cout << permuted_level_names << std::endl;
      throw std::invalid_argument(e.what());
    }
  }
  template <int M> NChildrenTree<M> get_subtree(const IndicesN<N> &idxs) const {
    try {
      auto subtree = internals::select_subtree_kv(nchildren_tree, idxs);
      return NChildrenTree<M>(std::move(subtree),
                              tail(level_names, idxs.size()));
    } catch (internals::KeyNotFoundError &e) {
      std::cout << "Indices invalid: " << std::endl;
      std::cout << idxs << std::endl;
      std::cout << level_names << std::endl;
      throw std::invalid_argument(e.what());
    }
  }
  int get_nchildren(const IndicesN<N> &idxs) const {
    try {
      return get_subtree(idxs).nchildren_tree->n.second;
    } catch (internals::KeyNotFoundError &e) {
      std::cout << "Indices invalid: " << std::endl;
      std::cout << idxs << std::endl;
      std::cout << level_names << std::endl;
      throw std::invalid_argument(e.what());
    }
  }

  const internals::KVTreeP<int> get_internal() const { return nchildren_tree; }

  friend SizeTree<N>::SizeTree(const NChildrenTree<N> &size_tree);
  SizeTree<N> size_tree() const { return SizeTree<N>(*this); }
};

template <int N> class PartitionTree {
private:
  Sizes sizes;
  PartList partitioners_list;
  internals::PartitionTree partition_tree;

public:
  PartitionTree(Sizes _sizes, PartList partitioners,
                vector<int> nonspatial_dimensions)
      : sizes(_sizes), partitioners_list(partitioners),
        partition_tree(internals::get_partition_treeM( //
            add_parity(_sizes, nonspatial_dimensions), //
            partitioners)){};

  vector<std::string> get_level_names() const {
    return internals::get_partitioners_names(partitioners_list);
  }

  IndicesN<N> get_indices(const Coordinates &coords) const {
    return internals::get_real_indices(partition_tree, coords);
  };
  vector<std::pair<int, IndicesN<N>>>
  get_indices_wg(const Coordinates &coords) const {

    auto indices_tree =
        internals::get_indices_tree_with_ghosts(partition_tree, coords);
    auto idxs_list = internals::get_relevant_indices_flat(indices_tree);
    return vtransform(idxs_list, [](auto f_idxs) {
      return std::make_pair(
          f_idxs.first, static_cast<IndicesN<N> &&>(std::move(f_idxs.second)));
    });
  };

  Coordinates get_coordinates(const IndicesN<N> &idxs) const {
    try {
      return internals::get_coord_from_idx(partition_tree, idxs);
    } catch (internals::KeyNotFoundError &e) {
      std::cout << "Indices invalid: " << std::endl;
      std::cout << idxs << std::endl;
      std::cout << get_level_names() << std::endl;
      throw std::invalid_argument(e.what());
    }
  }

  const internals::PartitionTree get_internal() const { return partition_tree; }

  friend NChildrenTree<N>::NChildrenTree(const PartitionTree<N> &pt);
  NChildrenTree<N> nchildren_tree() const { return NChildrenTree<N>(*this); }
};

// TODO: move elsewhere
template <class TreeIn, class TreeOut> class TreeLevelMatcher {

private:
  vector<int> map;

public:
  TreeLevelMatcher(TreeIn in, TreeOut out)
      : map(internals::find_permutation(in.get_level_names(),
                                        out.get_level_names())) {}
  template <class Container> Container operator()(Container in) {
    Container out(in.size());
    for (int i = 0; i < in.size(); ++i)
      out[map[i]] = in[i];
    return out;
  }
};

template <class TreeIn, class TreeB>
TreeLevelMatcher<TreeIn, TreeB> get_level_matcher(TreeIn in, TreeB out) {
  return TreeLevelMatcher<TreeIn, TreeB>(in, out);
}

} // namespace slow
} // namespace hypercubes

#endif // PARTITION_TREE_H_
