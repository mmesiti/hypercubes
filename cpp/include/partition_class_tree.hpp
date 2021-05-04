#ifndef PARTITION_CLASS_TREE_H_
#define PARTITION_CLASS_TREE_H_
#include "partitioners.hpp"
#include "tree.hpp"
#include <memory>
#include <string>

namespace hypercubes {
namespace slow {

using PartitionClassTree = TreeP<std::shared_ptr<IPartitioning>>;
using partitioners::PartList;

PartitionClassTree get_partition_class_tree(SizeParityD sp,
                                            const PartList &partitioners);

std::string partition_class_tree_to_str(PartitionClassTree t,
                                        std::string prefix, int max_level);

TreeP<int> get_indices_tree(PartitionClassTree t, Coordinates xs);

struct GhostResult {
  int idx;
  bool cached_flag;
  std::string name;
};

TreeP<GhostResult> get_indices_tree_with_ghosts(PartitionClassTree t,
                                                Coordinates xs);

} // namespace slow

} // namespace hypercubes

#endif // PARTITION_CLASS_TREE_H_
