#ifndef PARTITION_TREE_H_
#define PARTITION_TREE_H_
#include "partition_class_tree.hpp"
#include "partition_predicates.hpp"
#include "tree.hpp"
#include <map>

namespace hypercubes {
namespace slow {
TreeP<std::pair<int, int>> get_size_tree(const PartitionTree &t,
                                         PartitionPredicate p);

TreeP<std::pair<int, int>> get_size_tree(const PartitionTree &t,
                                         std::function<bool(Indices)> p);
TreeP<std::pair<int, int>>
get_offset_tree(const TreeP<std::pair<int, int>> &size_tree);

} // namespace slow
} // namespace hypercubes

#endif // PARTITION_TREE_H_
