#ifndef PARTITION_CLASS_TREE_H_
#define PARTITION_CLASS_TREE_H_
#include "partitioners.hpp"
#include "tree.hpp"
#include <map>
#include <memory>
#include <string>

namespace hypercubes {
namespace slow {

using PartitionClassTree = TreeP<std::shared_ptr<IPartitioning>>;
using partitioners::PartList;

class PCTBuilder {
  using Input = std::tuple<SizeParityD, PartList>;
  std::map<Input, PartitionClassTree> pct_cache;

public:
  PartitionClassTree operator()(SizeParityD sp, //
                                const PartList &partitioners);
};
std::string partition_class_tree_to_str(const PartitionClassTree &t, //
                                        const std::string &prefix,   //
                                        int max_level);

TreeP<int> get_indices_tree(const PartitionClassTree &t, //
                            const Coordinates &xs);

struct GhostResult {
  int idx;
  bool cached_flag;
  std::string name;
};

TreeP<GhostResult> get_indices_tree_with_ghosts(const PartitionClassTree &t,
                                                const Coordinates &xs);

vector<std::tuple<int, Indices>>
get_relevant_indices_flat(const TreeP<GhostResult> &tree_indices);

Coordinates get_coord_from_idx(const PartitionClassTree &t, //
                               const Indices &idx);

Sizes get_sizes_from_idx(const PartitionClassTree &t, const Indices &idx);
} // namespace slow

} // namespace hypercubes

#endif // PARTITION_CLASS_TREE_H_
