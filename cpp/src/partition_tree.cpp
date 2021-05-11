#include "partition_tree.hpp"
#include <memory>
namespace hypercubes {
namespace slow {

TreeP<std::pair<int, int>> get_size_tree(const PartitionTree &t,
                                         PartitionPredicate predicate) {

  auto binarified_predicate = [&predicate](Indices idx) {
    return to_bool(predicate(idx));
  };
  return get_size_tree(t, binarified_predicate);
}

TreeP<std::pair<int, int>> get_size_tree(const PartitionTree &t,
                                         std::function<bool(Indices)> predicate)

{

  using ResType = TreeP<std::pair<int, int>>;

  using F = std::function<ResType(const PartitionTree &, //
                                  const Indices &)>;

  auto getsize = [](ResType t) { return t->n.first; };

  F _get_size_tree = [&predicate,      //
                      &_get_size_tree, //
                      &getsize](const PartitionTree &pct,
                                const Indices &top_idxs) -> ResType {
    auto partition_class = pct->n;
    auto children = pct->children;
    vector<ResType> children_results;
    int nodesize = 0;
    if (children.size() != 0) {
      for (int idx = 0; idx < partition_class->max_idx_value(); ++idx) {
        auto new_pc = children[idx];
        Indices new_idxs = top_idxs;
        Indices predicate_idxs;
        new_idxs.push_back(idx);
        std::copy(new_idxs.begin() + 1, new_idxs.end(),
                  std::back_inserter(predicate_idxs));
        if (predicate(predicate_idxs)) {
          ResType r = _get_size_tree(new_pc, new_idxs);
          if (getsize(r) != 0)
            children_results.push_back(r);
        }
      }
      for (const auto &c : children_results)
        nodesize += getsize(c);
    } else
      nodesize = partition_class->max_idx_value();

    auto subn = std::make_pair(nodesize, //
                               *top_idxs.rbegin());
    return mt(subn, children_results);
  };

  return _get_size_tree(t, {0});
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
