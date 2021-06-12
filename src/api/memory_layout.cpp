#include "api/memory_layout.hpp"
#include "trees/kvtree.hpp"
#include "trees/level_swap.hpp"
#include "trees/partition_tree.hpp"
#include "trees/partition_tree_allocations.hpp"
#include "trees/tree.hpp"

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

Indices PartitionTree::get_indices(const Coordinates &coords) {
  return internals::get_real_indices(partition_tree, coords);
};
vector<std::pair<int, Indices>>
PartitionTree::get_indices_wg(const Coordinates &coords) {

  auto indices_tree =
      internals::get_indices_tree_with_ghosts(partition_tree, coords);
  return internals::get_relevant_indices_flat(indices_tree);
};

Coordinates PartitionTree::get_coordinates(const Indices &idxs) {
  return internals::get_coord_from_idx(partition_tree, idxs);
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

NChildrenTree::NChildrenTree(const PartitionTree &pt)
    : nchildren_tree(internals::get_nchildren_alloc_tree(pt.partition_tree)),
      level_names(internals::get_partitioners_names(pt.partitioners_list)) //
{}

NChildrenTree NChildrenTree::prune(const PartitionPredicate &predicate) {
  auto pruned_tree = internals::prune_tree(nchildren_tree, predicate);
  auto pruned_level_names = level_names;
  return NChildrenTree(std::move(pruned_tree), std::move(pruned_level_names));
}

NChildrenTree
NChildrenTree::permute(const vector<std::string> &permuted_level_names) const {
  auto permuted_tree = internals::swap_levels(
      nchildren_tree, //
      internals::find_permutation(permuted_level_names, level_names));

  vector<std::string> _pmlcp = permuted_level_names;
  return NChildrenTree(std::move(permuted_tree), std::move(_pmlcp));
}

const internals::KVTreeP<int> NChildrenTree::get_internal() const {
  return nchildren_tree;
}
NChildrenTree::NChildrenTree(internals::KVTreeP<int> &&_nct,
                             vector<std::string> &&_ln)
    : nchildren_tree(_nct), level_names(_ln) {}

NChildrenTree NChildrenTree::get_subtree(const Indices &idxs) const {

  auto subtree = internals::select_subtree_kv(nchildren_tree, idxs);
  vector<std::string> remaining_level_names;
  for (int i = idxs.size(); i < level_names.size(); ++i)
    remaining_level_names.push_back(level_names[i]);

  return NChildrenTree(std::move(subtree), std::move(remaining_level_names));
}

int NChildrenTree::get_nchildren(const Indices &idxs) const {
  return get_subtree(idxs).nchildren_tree->n.second;
}

SizeTree NChildrenTree::size_tree() const { return SizeTree(*this); }

// SizeTree

SizeTree::SizeTree(const NChildrenTree &nchildren_tree)
    : size_tree(internals::get_size_tree(nchildren_tree.nchildren_tree)) {}

const internals::KVTreeP<int> SizeTree::get_internal() const {
  return size_tree;
}

SizeTree SizeTree::get_subtree(const Indices &idxs) const {
  auto subtree = internals::select_subtree_kv(size_tree, idxs);
  return SizeTree(std::move(subtree));
}

SizeTree::SizeTree(internals::KVTreeP<int> &&st) : size_tree(st) {}

int SizeTree::get_size(const Indices &idxs) const {
  return get_subtree(idxs).size_tree->n.second;
}

OffsetTree SizeTree::offset_tree() const { return OffsetTree(*this); }

// OffsetTree
const internals::KVTreeP<int> OffsetTree::get_internal() const {
  return offset_tree;
}

OffsetTree::OffsetTree(const SizeTree &size_tree)
    : offset_tree(internals::get_offset_tree(size_tree.size_tree)) {}

OffsetTree OffsetTree::get_subtree(const Indices &idxs) const {
  auto subtree = internals::select_subtree_kv(offset_tree, idxs);
  return OffsetTree(std::move(subtree));
}

OffsetTree::OffsetTree(internals::KVTreeP<int> &&st) : offset_tree(st) {}

int OffsetTree::get_offset(const Indices &idxs) const {
  return get_subtree(idxs).offset_tree->n.second;
}

OffsetTree OffsetTree::shift(int shift) const {
  auto shifted_tree = internals::shift_tree(offset_tree, shift);
  return OffsetTree(std::move(shifted_tree));
}
Indices OffsetTree::get_indices(int offset) const {
  return internals::search_in_sorted_tree(offset_tree, offset);
}
