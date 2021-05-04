#include "partition_class_tree.hpp"
#include "tree.hpp"
#include <functional>

namespace hypercubes {
namespace slow {

// Note: not yet cached.
PartitionClassTree get_partition_class_tree(SizeParityD sp,
                                            const PartList &partitioners) {
  PartList other_partitioners;
  std::copy(partitioners.begin() + 1, partitioners.end(),
            std::back_inserter(other_partitioners));

  std::shared_ptr<IPartitioning> n = partitioners[0]->partition(sp);
  vector<PartitionClassTree> children;
  if (other_partitioners.size() != 0) {
    for (SizeParityD sp_child : n->sub_sizeparity_info_list()) {
      auto new_child = get_partition_class_tree(sp_child, other_partitioners);
      children.push_back(new_child);
    }
  }

  return mt(n, children);
}

std::string partition_class_tree_to_str(PartitionClassTree t,
                                        std::string prefix, int max_level) {

  std::string new_prefix = prefix + "   ";
  vector<std::string> children_results;
  if (t->children.size() != 0) {
    for (int i = 0; i < t->children.size() - 1; ++i)
      children_results.push_back(
          partition_class_tree_to_str(t->children[i],   //
                                      new_prefix + "|", //
                                      max_level - 1));
    children_results.push_back(
        partition_class_tree_to_str(*(t->children.rbegin()), //
                                    new_prefix + " ",        //
                                    max_level - 1));
  }
  if (max_level == 0)
    return "";
  std::string res;
  res += prefix + "+" + t->n->get_name() + "\n";
  res += prefix + " " + t->n->comments();
  for (std::string child_result : children_results)
    res += "\n" + child_result;
  return res;
}

TreeP<int> get_indices_tree(PartitionClassTree t, Coordinates xs) {

  auto partition_class = t->n;
  vector<IndexResultD> idxs = partition_class->coord_to_idxs(xs);
  IndexResultD real_idr =
      *std::find_if(idxs.begin(), //
                    idxs.end(),   //
                    [](IndexResultD i) { return not i.cached_flag; });

  vector<TreeP<int>> children_results;
  if (t->children.size() != 0) {
    int child_type = partition_class->idx_to_child_kind(real_idr.idx);
    children_results.push_back(
        get_indices_tree(t->children[child_type], real_idr.rest));
  }
  return mt(real_idr.idx, children_results);
}

TreeP<GhostResult> get_indices_tree_with_ghosts(PartitionClassTree t,
                                                Coordinates xs) {

  using F = std::function<TreeP<GhostResult>(int,                //
                                             bool,               //
                                             PartitionClassTree, //
                                             Coordinates,        //
                                             std::string)>;

  F _get_idtree_wghosts = [&_get_idtree_wghosts](int idx,              //
                                                 bool cached_flag,     //
                                                 PartitionClassTree t, //
                                                 Coordinates xs,       //
                                                 std::string name) {
    auto partition_class = t->n;
    vector<IndexResultD> idrs = partition_class->coord_to_idxs(xs);
    vector<TreeP<GhostResult>> children_results;
    for (IndexResultD idr : idrs) {
      if (t->children.size() != 0) {
        int child_type = partition_class->idx_to_child_kind(idr.idx);
        children_results.push_back(
            _get_idtree_wghosts(idr.idx,                 //
                                idr.cached_flag,         //
                                t->children[child_type], //
                                idr.rest,                //
                                partition_class->get_name()));
      } else {
        GhostResult gc{idr.idx, idr.cached_flag, partition_class->get_name()};
        children_results.push_back(mt(gc, {}));
      }
    }
    GhostResult g{idx, false, name};
    return mt(g, children_results);
  };

  return _get_idtree_wghosts(0, false, t, xs, "ROOT");
}
} // namespace slow
} // namespace hypercubes
