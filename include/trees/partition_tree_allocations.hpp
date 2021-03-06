#ifndef PARTITION_TREE_ALLOCATIONS_H_
#define PARTITION_TREE_ALLOCATIONS_H_
#include "partition_tree.hpp"
#include "selectors/partition_predicates.hpp"
#include "tree_data_structure.hpp"
#include <map>

namespace hypercubes {
namespace slow {
namespace internals {

template <class Value> using KVTreeP = TreeP<std::pair<int, Value>>;

KVTreeP<int> get_nchildren_alloc_tree(const PartitionTree &t);
KVTreeP<int> get_skeleton_tree(const PartitionTree &t);

/// A <tot alloc size of subtree, idx in full tree> conditionally-allocated tree
KVTreeP<int> get_size_tree(const TreeP<std::pair<int, int>> &max_idx_tree);

KVTreeP<int> get_offset_tree(const TreeP<std::pair<int, int>> &size_tree);

int get_offset(const TreeP<std::pair<int, int>> &offset_tree, const Indices &);
Indices get_indices(const TreeP<std::pair<int, int>> &offset_tree, int offset);

} // namespace internals
} // namespace slow
} // namespace hypercubes

#endif // PARTITION_TREE_H_
