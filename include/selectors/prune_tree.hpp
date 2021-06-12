#ifndef PRUNE_TREE_H_
#define PRUNE_TREE_H_
#include "partition_predicates.hpp"
#include "trees/kvtree.hpp"

namespace hypercubes {
namespace slow {
namespace internals {

// TODO: (maybe) specialise to int and move to source instead
template <class Value>
KVTreeP<Value> prune_tree(const KVTreeP<Value> &t, //
                          const PartitionPredicate &predicate) {
  using Tree = TreeP<std::pair<int, Value>>;

  auto _prune_tree = [&predicate](const Tree &t,           //
                                  const Indices &top_idxs, //
                                  auto f) -> Tree {
    vector<Tree> children;
    for (int idx = 0; idx < t->children.size(); ++idx) {
      Indices new_idxs = append(top_idxs, idx);
      switch (predicate(tail(new_idxs))) {
      case BoolM::T:
        children.push_back(t->children[idx]);
        break;
      case BoolM::M:
        children.push_back(f(t->children[idx], new_idxs, f));
        break;
      default:
        break;
      }
    }
    return mt(t->n, children);
  };

  return _prune_tree(t, {0}, _prune_tree);
}
} // namespace internals
} // namespace slow
} // namespace hypercubes
#endif // PRUNE_TREE_H_
