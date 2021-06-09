#include "trees/partition_tree_allocations.hpp"
#include "trees/kvtree.hpp"
#include "utils/utils.hpp"
#include <memory>
namespace hypercubes {
namespace slow {
namespace internals {

namespace get_nchildren_alloc_tree_detail {
bool nonzero(std::pair<int, int> node) {
  int alloc_children_count = node.second;
  return alloc_children_count > 0;
};

} // namespace get_nchildren_alloc_tree_detail

TreeP<std::pair<int, int>>
get_nchildren_alloc_tree(const PartitionTree &t, PartitionPredicate predicate) {

  auto max_idx_tree = get_max_idx_treeM(t);
  auto max_idx_tree_kv = number_childrenM(max_idx_tree);
  auto max_idx_tree_pruned = prune_tree(max_idx_tree_kv, predicate);
  return max_idx_tree_pruned;
}

TreeP<std::pair<int, int>>
get_size_tree(const TreeP<std::pair<int, int>> &max_idx_tree) {

  using InOutType = TreeP<std::pair<int, int>>;

  auto getsize = [](InOutType t) { return t->n.second; };
  auto getidx = [](InOutType t) { return t->n.first; };

  auto _get_size_tree = [&getsize, &getidx](const InOutType &max_idx_tree,
                                            auto &f) -> InOutType {
    auto max_alloc_and_idx = max_idx_tree->n;
    auto children = max_idx_tree->children;
    vector<InOutType> children_results;
    int nodesize = 0;
    if (children.size() != 0) {
      for (const auto &c : children) {
        auto new_pc = c;
        InOutType r = f(new_pc, f);
        if (getsize(r) != 0)
          children_results.push_back(r);
      }
      for (const auto &c : children_results)
        nodesize += getsize(c);
    } else
      nodesize = getsize(max_idx_tree);

    auto subn = std::make_pair(getidx(max_idx_tree), //
                               nodesize);
    return mt(subn, children_results);
  };

  return _get_size_tree(max_idx_tree, _get_size_tree);
}

TreeP<std::pair<int, int>>
get_offset_tree(const TreeP<std::pair<int, int>> &size_tree) {
  using SizeTree = TreeP<std::pair<int, int>>;
  using ResType = TreeP<std::pair<int, int>>;

  using F = std::function<ResType(const SizeTree &, //
                                  int)>;
  F _get_start_tree = [&_get_start_tree](const SizeTree &st, //
                                         int start) -> ResType {
    vector<ResType> children_results;
    int _idx_so_far = start;
    for (const auto &child : st->children) {
      children_results.push_back(_get_start_tree(child, _idx_so_far));
      _idx_so_far += child->n.second;
    }
    return mt(std::make_pair(st->n.first, start), children_results);
  };
  return _get_start_tree(size_tree, 0);
}

int get_offset(const TreeP<std::pair<int, int>> &offset_tree, //
               const Indices &idxs) {
  auto children = offset_tree->children;
  int key = idxs[0];
  if (idxs.size() == 1)
    return key + offset_tree->n.second;
  else {
    auto child =
        std::find_if(children.begin(), //
                     children.end(),   //
                     [key](auto child) { return child->n.first == key; });
    return get_offset(*child, tail(idxs));
  }
}
Indices get_indices(const TreeP<std::pair<int, int>> &offset_tree, //
                    int offset) {

  auto _get_indices = [](const TreeP<std::pair<int, int>> &offset_tree,
                         int offset, auto frec) {
    auto children = offset_tree->children;
    if (children.size() == 0) {
      return Indices{(offset - offset_tree->n.second)};
    } else {
      auto child_after = std::find_if(children.begin(),      //
                                      children.end(),        //
                                      [offset](auto child) { //
                                        return offset < child->n.second;
                                      });
      auto child = *(child_after - 1);
      return append(child->n.first, frec(child, offset, frec));
    }
  };
  return _get_indices(offset_tree, offset, _get_indices);
}

} // namespace internals
} // namespace slow
} // namespace hypercubes
