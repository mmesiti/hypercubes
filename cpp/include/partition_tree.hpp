#ifndef PARTITION_TREE_H_
#define PARTITION_TREE_H_

#include "partition_class_tree.hpp"
#include "partition_predicates.hpp"
#include "tree.hpp"

namespace hypercubes {
namespace slow {
TreeP<std::pair<int, int>> get_size_tree(const PartitionClassTree &t,
                                         PartitionPredicate p);
TreeP<int> get_start_tree(const TreeP<std::pair<int, int>> &size_tree);
vector<int> get_start_offsets(const TreeP<std::pair<int, int>> &size_tree);

Indices next(const TreeP<std::pair<int, int>> &size_tree, const Indices &idxs);

} // namespace slow
} // namespace hypercubes

#endif // PARTITION_TREE_H_
