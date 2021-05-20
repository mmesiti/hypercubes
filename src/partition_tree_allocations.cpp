#include "partition_tree_allocations.hpp"
#include "utils.hpp"
#include <memory>
namespace hypercubes {
namespace slow {

TreeP<std::pair<int, int>>
get_nchildren_alloc_tree(const PartitionTree &t,
                         std::function<bool(Indices)> predicate) {

  using ResType = TreeP<std::pair<int, int>>;

  auto get_n_alloc_children = [](ResType t) { return t->n.first; };

  // TODO: merge with get_max_idx_tree
  auto _get_max_idx_tree = [&predicate, //
                            &get_n_alloc_children](const PartitionTree &pct,
                                                   const Indices &top_idxs,
                                                   auto &f) -> ResType {
    auto partition_class = pct->n;
    auto children = pct->children;
    vector<ResType> children_results;
    int n_allocated_children = 0;
    if (children.size() != 0) {
      Indices predicate_idxs = tail(top_idxs);
      if (predicate(predicate_idxs)) {
        for (int idx = 0; idx < partition_class->max_idx_value(); ++idx) {
          auto new_pc = children[idx];
          Indices new_idxs = append(top_idxs, idx);
          ResType r = f(new_pc, new_idxs, f);
          if (get_n_alloc_children(r) != 0)
            children_results.push_back(r);
        }
      }
      n_allocated_children = children_results.size();
    } else
      n_allocated_children = partition_class->max_idx_value();

    int idx_in_full_tree = *top_idxs.rbegin();
    auto subn = std::make_pair(n_allocated_children, //
                               idx_in_full_tree);
    return mt(subn, children_results);
  };

  return _get_max_idx_tree(t, {0}, _get_max_idx_tree);
}

TreeP<std::pair<int, int>>
get_nchildren_alloc_tree(const PartitionTree &t, PartitionPredicate predicate) {

  auto binarified_predicate = [&predicate](Indices idx) {
    return to_bool(predicate(idx));
  };
  return get_nchildren_alloc_tree(t, binarified_predicate);
}

TreeP<std::pair<int, int>>
get_size_tree(const TreeP<std::pair<int, int>> &max_idx_tree) {

  using InOutType = TreeP<std::pair<int, int>>;

  auto getsize = [](InOutType t) { return t->n.first; };
  auto getidx = [](InOutType t) { return t->n.second; };

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
      nodesize = max_alloc_and_idx.first;

    auto subn = std::make_pair(nodesize, //
                               max_alloc_and_idx.second);
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
      _idx_so_far += child->n.first;
    }
    return mt(std::make_pair(start, st->n.second), children_results);
  };
  return _get_start_tree(size_tree, 0);
}

} // namespace slow
} // namespace hypercubes
