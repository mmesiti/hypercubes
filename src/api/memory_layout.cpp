#include "api/memory_layout.hpp"
#include "trees/kvtree.hpp"
#include "trees/level_swap.hpp"
#include "trees/partition_tree.hpp"
#include "trees/partition_tree_allocations.hpp"
#include "trees/tree.hpp"

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

// NChildrenTree
NChildrenTree::NChildrenTree(const PartitionTree &pt,
                             const PartitionPredicate &predicate)
    : nchildren_tree(internals::get_nchildren_alloc_tree(pt.partition_tree, //
                                                         predicate)),
      level_names(internals::get_partitioners_names(pt.partitioners_list)) //
{}

NChildrenTree
NChildrenTree::permute(const vector<std::string> &permuted_level_names) const {
  auto permuted_tree =
      internals::swap_levels(nchildren_tree,                          //
                             internals::find_permutation(level_names, //
                                                         permuted_level_names));

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
SizeTree::SizeTree(const NChildrenTree &nchildren_tree) {}

OffsetTree::OffsetTree(const SizeTree &size_tree) {}
int OffsetTree::get_offset(const Indices &idxs) const { return 0; }
Indices OffsetTree::get_indices(int offset) const { return Indices(); }
