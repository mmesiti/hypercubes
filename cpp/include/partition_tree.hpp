#ifndef PARTITION_CLASS_TREE_H_
#define PARTITION_CLASS_TREE_H_
#include "partitioners.hpp"
#include "tree.hpp"
#include <map>
#include <memory>
#include <string>

namespace hypercubes {
namespace slow {
using partitioners::PartList;
using PartitionTree = TreeP<std::shared_ptr<IPartitioning>>;

class PTBuilder {
  using Input = std::tuple<SizeParityD, PartList>;
  std::map<Input, PartitionTree> pct_cache;

public:
  PartitionTree operator()(SizeParityD sp, //
                           const PartList &partitioners);
};
std::string partition_class_tree_to_str(const PartitionTree &t,    //
                                        const std::string &prefix, //
                                        int max_level);

TreeP<int> get_indices_tree(const PartitionTree &t, //
                            const Coordinates &xs);

struct GhostResult {
  int idx;
  bool cached_flag;
  std::string name;
};

/* Returns a tree of indices representing the coordinates.
 * The root of the tree is represented as an additional 0
 * that does not correspond to any existing level. */
TreeP<GhostResult> get_indices_tree_with_ghosts(const PartitionTree &t,
                                                const Coordinates &xs);

/* Takes the output of get_indices_tree_with_ghosts
 *and returns a list of pairs "cached flag count - indices".
 * The "0" prefix from get_indices_tree_with_ghosts is removed. */
vector<std::tuple<int, Indices>>
get_relevant_indices_flat(const TreeP<GhostResult> &tree_indices);

Coordinates get_coord_from_idx(const PartitionTree &t, //
                               const Indices &idx);

Sizes get_sizes_from_idx(const PartitionTree &t, const Indices &idx);

TreeP<int> get_max_idx_tree(const PartitionTree &t);

bool validate_idx(const PartitionTree &t, const Indices &idx);

vector<std::pair<int, int>> get_partition_limits(const PartitionTree &t, //
                                                 const Indices &idx);
} // namespace slow

} // namespace hypercubes

#endif // PARTITION_CLASS_TREE_H_
