#include "api/memory_layout.hpp"
#include "exceptions/exceptions.hpp"
#include "selectors/prune_tree.hpp"
#include "trees/kvtree.hpp"
#include "trees/level_swap.hpp"
#include "trees/partition_tree.hpp"
#include "trees/partition_tree_allocations.hpp"
#include "trees/tree.hpp"
#include <stdexcept>
#include <iostream> // DEBUG

/******************
 * PARTITION TREE *
 ******************/

using namespace hypercubes::slow;
PartitionTree::PartitionTree(Sizes _sizes, PartList partitioners,
                             vector<int> nonspatial_dimensions)
    : sizes(_sizes), partitioners_list(partitioners),
      partition_tree(internals::get_partition_treeM( //
          add_parity(_sizes, nonspatial_dimensions), //
          partitioners)){};

vector<std::string> PartitionTree::get_level_names() const {
  return internals::get_partitioners_names(partitioners_list);
}

Indices PartitionTree::get_indices(const Coordinates &coords) const {
  return internals::get_real_indices(partition_tree, coords);
};
vector<std::pair<int, Indices>>
PartitionTree::get_indices_wg(const Coordinates &coords) const {

  auto indices_tree =
      internals::get_indices_tree_with_ghosts(partition_tree, coords);
  return internals::get_relevant_indices_flat(indices_tree);
};

Coordinates PartitionTree::get_coordinates(const Indices &idxs) const {
  try {
    return internals::get_coord_from_idx(partition_tree, idxs);
  } catch (internals::KeyNotFoundError &e) {
    std::cout << "Indices invalid: " << std::endl;
    std::cout << idxs << std::endl;
    std::cout << get_level_names() << std::endl;
    throw std::invalid_argument(e.what());
  }
}

const internals::PartitionTree PartitionTree::get_internal() const {
  return partition_tree;
}

NChildrenTree PartitionTree::nchildren_tree() const {
  return NChildrenTree(*this);
}

/******************
 * NCHILDREN TREE *
 ******************/
NChildrenTree::NChildrenTree(internals::KVTreeP<int> &&_nct,
                             vector<std::string> &&_ln)
    : nchildren_tree(_nct), level_names(_ln) {}

vector<std::string> NChildrenTree::get_level_names() const {
  return level_names;
}

NChildrenTree::NChildrenTree(const PartitionTree &pt)
    : nchildren_tree(internals::get_nchildren_alloc_tree(pt.partition_tree)),
      level_names(internals::get_partitioners_names(pt.partitioners_list)) //
{}

NChildrenTree NChildrenTree::prune(const PartitionPredicate &predicate) const {
  auto pruned_tree = internals::prune_tree(nchildren_tree, predicate);
  auto pruned_level_names = level_names;
  return NChildrenTree(std::move(pruned_tree), std::move(pruned_level_names));
}
NChildrenTree
NChildrenTree::permute(const vector<std::string> &permuted_level_names) const {
  try {

    auto permuted_tree = internals::swap_levels(
        nchildren_tree, //
        internals::find_permutation(permuted_level_names, level_names));
    vector<std::string> _pmlcp = permuted_level_names;
    return NChildrenTree(std::move(permuted_tree), std::move(_pmlcp));
  } catch (const internals::KeyNotFoundError &e) {
    std::cout << "Error in swapping levels in the tree:" << std::endl;
    std::cout << level_names << std::endl;
    std::cout << "->" << std::endl;
    std::cout << permuted_level_names << std::endl;
    throw std::invalid_argument(e.what());
  }
}
NChildrenTree NChildrenTree::get_subtree(const Indices &idxs) const {
  try {
    auto subtree = internals::select_subtree_kv(nchildren_tree, idxs);
    return NChildrenTree(std::move(subtree), tail(level_names, idxs.size()));
  } catch (internals::KeyNotFoundError &e) {
    std::cout << "Indices invalid: " << std::endl;
    std::cout << idxs << std::endl;
    std::cout << level_names << std::endl;
    throw std::invalid_argument(e.what());
  }
}
int NChildrenTree::get_nchildren(const Indices &idxs) const {
  try {
    return get_subtree(idxs).nchildren_tree->n.second;
  } catch (internals::KeyNotFoundError &e) {
    std::cout << "Indices invalid: " << std::endl;
    std::cout << idxs << std::endl;
    std::cout << level_names << std::endl;
    throw std::invalid_argument(e.what());
  }
}

const internals::KVTreeP<int> NChildrenTree::get_internal() const {
  return nchildren_tree;
}

SizeTree NChildrenTree::size_tree() const { return SizeTree(*this); }

/************
 * SizeTree *
 ************/

SizeTree::SizeTree(internals::KVTreeP<int> &&st, vector<std::string> &&ln)
    : size_tree(st), level_names(ln) {}
vector<std::string> SizeTree::get_level_names() const { return level_names; }
SizeTree::SizeTree(const NChildrenTree &nchildren_tree)
    : size_tree(internals::get_size_tree(nchildren_tree.nchildren_tree)),
      level_names(nchildren_tree.level_names) {}
const internals::KVTreeP<int> SizeTree::get_internal() const {
  return size_tree;
}
SizeTree SizeTree::get_subtree(const Indices &idxs) const {
  try {
    auto subtree = internals::select_subtree_kv(size_tree, idxs);
    return SizeTree(std::move(subtree), tail(level_names, idxs.size()));
  } catch (internals::KeyNotFoundError &e) {
    std::cout << "Indices invalid: " << std::endl;
    std::cout << idxs << std::endl;
    std::cout << level_names << std::endl;
    throw std::invalid_argument(e.what());
  }
}
int SizeTree::get_size(const Indices &idxs) const {
  try {
    return get_subtree(idxs).size_tree->n.second;
  } catch (internals::KeyNotFoundError &e) {
    std::cout << "Indices invalid: " << std::endl;
    std::cout << idxs << std::endl;
    std::cout << level_names << std::endl;
    throw std::invalid_argument(e.what());
  }
}
OffsetTree SizeTree::offset_tree() const { return OffsetTree(*this); }

/**************
 * OffsetTree *
 **************/
const internals::KVTreeP<int> OffsetTree::get_internal() const {
  return offset_tree;
}

OffsetTree::OffsetTree(const SizeTree &size_tree)
    : offset_tree(internals::get_offset_tree(size_tree.size_tree)),
      level_names(size_tree.level_names) {}
vector<std::string> OffsetTree::get_level_names() const { return level_names; }

OffsetTree OffsetTree::get_subtree(const Indices &idxs) const {
  try {
    auto subtree = internals::select_subtree_kv(offset_tree, idxs);
    return OffsetTree(std::move(subtree), tail(level_names, idxs.size()));
  } catch (internals::KeyNotFoundError &e) {
    std::cout << "Indices invalid: " << std::endl;
    std::cout << idxs << std::endl;
    std::cout << level_names << std::endl;
    throw std::invalid_argument(e.what());
  }
}

OffsetTree::OffsetTree(internals::KVTreeP<int> &&st, vector<std::string> &&ln)
    : offset_tree(st), level_names(ln) {}

int OffsetTree::get_offset(const Indices &idxs) const {
  try {
    return get_subtree(idxs).offset_tree->n.second;
  } catch (internals::KeyNotFoundError &e) {
    std::cout << "Indices invalid: " << std::endl;
    std::cout << idxs << std::endl;
    std::cout << level_names << std::endl;
    throw std::invalid_argument(e.what());
  }
}

OffsetTree OffsetTree::shift(int shift) const {
  auto shifted_tree = internals::shift_tree(offset_tree, shift);
  auto ln = level_names;
  return OffsetTree(std::move(shifted_tree), std::move(ln));
}
Indices OffsetTree::get_indices(int offset) const {
  return internals::search_in_sorted_tree(offset_tree, offset);
}
