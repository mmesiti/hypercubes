#ifndef PARTITION_TREE_H_
#define PARTITION_TREE_H_
#include "partition_predicates.hpp"
#include "partition_tree.hpp"
#include "tree.hpp"
#include <map>

namespace hypercubes {
namespace slow {
/// A <n_alloc_children, idx in full tree> conditionally-allocated tree
TreeP<std::pair<int, int>>
get_nchildren_alloc_tree(const PartitionTree &t,
                         std::function<bool(Indices)> p);
TreeP<std::pair<int, int>>
get_nchildren_alloc_tree(const PartitionTree &t, PartitionPredicate predicate);

/// A <tot alloc size of subtree, idx in full tree> conditionally-allocated tree
TreeP<std::pair<int, int>>
get_size_tree(const TreeP<std::pair<int, int>> &max_idx_tree);

TreeP<std::pair<int, int>>
get_offset_tree(const TreeP<std::pair<int, int>> &size_tree);

} // namespace slow
} // namespace hypercubes

#endif // PARTITION_TREE_H_
