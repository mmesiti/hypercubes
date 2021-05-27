#ifndef KVTREE_H_
#define KVTREE_H_
#include "tree.hpp"
#include "tree_data_structure.hpp"
#include "utils.hpp"
#include <functional>
#include <memoisation_details.hpp>

namespace hypercubes {
namespace slow {

template <class Value>
TreeP<std::pair<int, Value>> fix_key(const TreeP<std::pair<int, Value>> &t,
                                     int new_key) {
  return mt(std::make_pair(new_key, t->n.second), t->children);
}

template <class V>
TreeP<std::pair<int, V>>                             //
collapse_level(const TreeP<std::pair<int, V>> &tree, //
               int level_to_collapse,                //
               int child_to_replace) {
  auto solder_subtree_in = [](auto c, auto tree) {
    return fix_key(c, tree->n.first);
  };
  return _collapse_level(tree,              //
                         level_to_collapse, //
                         child_to_replace,  //
                         solder_subtree_in);
}
template <class Value>
TreeP<std::pair<int, Value>> //
bring_level_on_top(const TreeP<std::pair<int, Value>> &tree, int level) {
  using Node = std::pair<int, Value>;
  using TreeP = TreeP<Node>;
  auto get_subtree_features = [](TreeP c) {
    vector<int> children_keys_at_level =
        vtransform(c->children, [](TreeP t) { return t->n.first; });
    return std::make_pair(c->n.second, //
                          children_keys_at_level);
  };
  auto get_nchildren_from_other_features =
      [](vector<int> children_keys_at_level) {
        return children_keys_at_level.size();
      };
  auto fix_tree_indices = [](TreeP new_subtree,
                             vector<int> children_keys_at_level,
                             int child_to_pick) {
    return fix_key(new_subtree, children_keys_at_level[child_to_pick]);
  };
  auto create_top_node = [](Value new_top) {
    return std::make_pair(0, new_top);
  };

  return _bring_level_on_top(tree,                              //
                             level,                             //
                             get_subtree_features,              //
                             get_nchildren_from_other_features, //
                             fix_tree_indices,                  //
                             create_top_node);
}

template <class Value>
TreeP<std::pair<int, Value>>
swap_levels(const TreeP<std::pair<int, Value>> &tree,
            const vector<int> &new_level_ordering) {
  using Node = std::pair<int, Value>;
  auto fix_new_tree = [](const TreeP<Node> &new_tree, //
                         const TreeP<Node> &tree) {
    return fix_key(new_tree, tree->n.first);
  };

  return _swap_levels(tree, new_level_ordering, fix_new_tree);
}

namespace number_children_detail {

template <class Value> using Out = TreeP<std::pair<int, Value>>;

template <class Value>
Out<Value> base(                                          //
    std::function<Out<Value>(const TreeP<Value> &)> frec, //
    const TreeP<Value> &tree) {
  using Node = std::pair<int, Value>;
  using ResType = TreeP<Node>;

  Node new_root = std::make_pair(0, tree->n);
  int ichild = 0;
  vector<ResType> new_children =
      vtransform(tree->children, //
                 [&ichild, &frec](auto c) {
                   ResType new_child = frec(c);
                   Node new_root = std::make_pair(ichild++, //
                                                  new_child->n.second);
                   return mt(new_root, new_child->children);
                 });
  return mt(new_root, new_children);
}

} // namespace number_children_detail
template <class Value>
TreeP<std::pair<int, Value>> number_children(const TreeP<Value> &tree,
                                             bool memoised = false) {
  if (memoised) {
    return number_children_detail::memoised(tree);
  } else {
    return number_children_detail::nomemo(tree);
  }
}

namespace prune_tree_details {

template <class Value> using Tree = TreeP<std::pair<int, Value>>;
using Predicate = std::function<bool(vector<int>)>;
using Indices = vector<int>;
template <class Value>
Tree<Value> base_wp(
    std::function<Tree<Value>(const Tree<Value> &, const Indices &)> frec, //
    const Tree<Value> &t,                                                  //
    const Indices &top_idxs,                                               //
    Predicate predicate) {
  vector<Tree<Value>> children;
  for (int idx = 0; idx < t->children.size(); ++idx) {
    Indices new_idxs = append(top_idxs, idx);
    if (predicate(tail(new_idxs)))
      children.push_back(frec(t->children[idx], new_idxs));
  }
  return mt(t->n, children);
}

} // namespace prune_tree_details

template <class Value>
TreeP<std::pair<int, Value>>
prune_tree(const TreeP<std::pair<int, Value>> &t,
           std::function<bool(vector<int>)> predicate, bool memoised = false) {
  if (memoised) {
    return prune_tree_details::memoised(t, {0}, predicate);
  } else {
    return prune_tree_details::nomemo(t, {0}, predicate);
  }
}

} // namespace slow
} // namespace hypercubes

#endif // KVTREE_H_
