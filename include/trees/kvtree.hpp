#ifndef KVTREE_H_
#define KVTREE_H_
#include "exceptions/exceptions.hpp"
#include "memoisation/memoisation_details.hpp"
#include "selectors/partition_predicates.hpp"
#include "tree.hpp"
#include "tree_data_structure.hpp"
#include "utils/utils.hpp"
#include <functional>
#include <set>

namespace hypercubes {
namespace slow {
namespace internals {

template <class Value> using KVTreeP = TreeP<std::pair<int, Value>>;
template <class A, class B> std::pair<A, B> mp(A a, B b) {
  return std::make_pair(a, b);
}

template <class Value>
KVTreeP<Value> fix_key(const KVTreeP<Value> &t, int new_key) {
  return mt(std::make_pair(new_key, t->n.second), t->children);
}

template <class Value>
KVTreeP<Value>                             //
collapse_level(const KVTreeP<Value> &tree, //
               int level_to_collapse,      //
               int child_to_replace) {
  auto solder_subtree_in = [](auto c, auto tree) {
    return fix_key(c, tree->n.first);
  };
  return _collapse_level(tree,              //
                         level_to_collapse, //
                         child_to_replace,  //
                         solder_subtree_in);
}

// TODO: consider memoisation
template <class Value>
KVTreeP<Value>                                           //
collapse_level_by_key_robust(const KVTreeP<Value> &tree, //
                             int level_to_collapse,      //
                             int child_key_to_replace) {
  if (level_to_collapse == 0) {
    auto c_it =
        std::find_if(tree->children.begin(), //
                     tree->children.end(),   //
                     [child_key_to_replace](const KVTreeP<Value> &child) {
                       return child->n.first == child_key_to_replace;
                     });
    if (c_it == tree->children.end())
      return KVTreeP<Value>(); // TODO: CHECK - hopefully null pointer?
    else
      return fix_key(*c_it, tree->n.first);
  } else {
    vector<KVTreeP<Value>> children_and_holes = vtransform(
        tree->children, //
        [level_to_collapse, child_key_to_replace](const KVTreeP<Value> &c) {
          return collapse_level_by_key_robust(c,                     //
                                              level_to_collapse - 1, //
                                              child_key_to_replace);
        });

    vector<KVTreeP<Value>> children;
    children.reserve(tree->children.size());
    // Select non-null only
    std::copy_if(children_and_holes.begin(),   //
                 children_and_holes.end(),     //
                 std::back_inserter(children), //
                 [](const KVTreeP<Value> &c) { return c != KVTreeP<Value>(); });
    return mt(tree->n, children);
  }
}

template <class Value>
KVTreeP<Value> //
bring_level_on_top(const KVTreeP<Value> &tree, int level) {
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
KVTreeP<Value> bring_level_on_top_robust(const KVTreeP<Value> &tree, //
                                         int level) {

  using TreeP = KVTreeP<Value>;
  TreeP top_tree = truncate_tree(tree, level + 2);
  vector<TreeP> subtrees = get_flat_list_of_subtrees(top_tree, level);
  Value value = subtrees[0]->n.second;
  // check that all the values are equal.
  for (int itree = 0; itree < subtrees.size(); ++itree) {
    if (subtrees[itree]->n.second != value) {
      std::stringstream message;
      message << "Not all subtrees are equivalent:" << std::endl
              << value
              << std::endl
              //<< n_and_cs << std::endl;
              << "different one is " << itree
              << " out of total number of subtrees:" << subtrees.size()
              << std::endl
              << "level: " << level << std::endl;
      throw TreeLevelPermutationError(message.str().c_str());
    }
  }

  std::set<int> children_key_set;
  {
    auto all_children_key_sets =
        vtransform(subtrees,            //
                   [](const TreeP &t) { //
                     auto keyvec =
                         vtransform(t->children, //
                                    [](const TreeP &c) { return c->n.first; });
                     std::set<int> keyset;
                     for (int key : keyvec)
                       keyset.insert(key);
                     return keyset;
                   });

    for (const auto &child_key_set : all_children_key_sets)
      for (const auto &child_key : child_key_set)
        children_key_set.insert(child_key);
  }

  vector<TreeP> new_subtrees;
  for (int child_to_pick : children_key_set) {
    TreeP new_subtree =
        collapse_level_by_key_robust(tree, level, child_to_pick);
    TreeP new_subtree_transformed = fix_key(new_subtree, child_to_pick);

    new_subtrees.push_back(new_subtree_transformed);
  }
  return mt(mp(0, value), new_subtrees);
}

template <class Value>
KVTreeP<Value> swap_levels(const KVTreeP<Value> &tree,
                           const vector<int> &new_level_ordering) {
  using Node = std::pair<int, Value>;
  auto fix_new_tree = [](const TreeP<Node> &new_tree, //
                         const TreeP<Node> &tree) {
    return fix_key(new_tree, tree->n.first);
  };

  return _swap_levels(tree, new_level_ordering, fix_new_tree,
                      bring_level_on_top_robust<Value>);
}

namespace memodetails {
namespace number_children {

template <class Value> using Out = KVTreeP<Value>;

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

} // namespace number_children
} // namespace memodetails
template <class Value>
KVTreeP<Value> number_childrenM(const TreeP<Value> &tree) {
  using namespace memodetails::number_children;
  return Memo<Value>().memoised(tree);
}

template <class Value>
KVTreeP<Value> number_children(const TreeP<Value> &tree) {
  using namespace memodetails::number_children;
  return Memo<Value>().nomemo(tree);
}

namespace select_subtree_details {

template <class Value>
const KVTreeP<Value> select_key(const vector<KVTreeP<Value>> &children,
                                int child_idx) {

  auto child_iter = std::find_if(
      children.begin(), //
      children.end(),   //
      [child_idx](auto child) { return child->n.first == child_idx; });

  if (child_iter == children.end()) {
    std::stringstream message;
    message << "child_idx = " << child_idx //
            << "not found in keys:"
            << vtransform(children, [](auto c) { return c->n.first; }) //
            << std::endl;
    throw KeyNotFoundError(message.str().c_str());
  }
  return *child_iter;
}
} // namespace select_subtree_details
template <class Value, class Indices>
const KVTreeP<Value> select_subtree_kv(const KVTreeP<Value> &tree,
                                       const Indices &idxs) {

  using namespace select_subtree_details;
  return base<std::pair<int, Value>, Indices, select_key<Value>>(tree, idxs);
}

namespace memodetails {
namespace shift_tree {

template <class Value>
KVTreeP<Value>
base(std::function<KVTreeP<Value>(const KVTreeP<Value> &, Value)> frec, //
     const KVTreeP<Value> &tree, Value shift) {

  int key = tree->n.first;
  Value val = tree->n.second;

  vector<KVTreeP<Value>> children_results =
      vtransform(tree->children,          //
                 [&frec, shift](auto c) { //
                   return frec(c, shift);
                 }); //

  return mt(std::make_pair(key, val + shift), children_results);
}

} // namespace shift_tree
} // namespace memodetails

template <class Value>
KVTreeP<Value> shift_treeM(const KVTreeP<Value> &tree, Value shift) {
  using namespace memodetails::shift_tree;
  return Memo<Value>().memoised(tree, shift);
}

template <class Value>
KVTreeP<Value> shift_tree(const KVTreeP<Value> &tree, Value shift) {
  using namespace memodetails::shift_tree;
  return Memo<Value>().nomemo(tree, shift);
}

template <class SortableValue>
Indices search_in_sorted_tree(const KVTreeP<SortableValue> &tree,
                              SortableValue x) {

  Indices res;
  auto _search = [](const KVTreeP<SortableValue> &tree, SortableValue x,
                    Indices &r, auto frec) -> Indices {
    auto &children = tree->children;
    if (children.size() == 0) {
      if (tree->n.second == x)
        return r;
      else {
        std::stringstream message;
        message << "Not found: " << x << " != " << tree->n.second << std::endl;
        throw KeyNotFoundError(message.str().c_str());
      }
    }
    auto c_itp = std::find_if(tree->children.begin(), //
                              tree->children.end(),   //
                              [x](auto c) { return c->n.second > x; });
    if (c_itp == children.begin()) {
      std::stringstream message;
      message << "Not found: " << x << " < " << children[0]->n.second
              << std::endl;
      throw KeyNotFoundError(message.str().c_str());
    }
    auto c_it = c_itp - 1;
    r.push_back((*c_it)->n.first);
    return frec(*c_it, x, r, frec);
  };

  return _search(tree, x, res, _search);
}

} // namespace internals
} // namespace slow
} // namespace hypercubes

#endif // KVTREE_H_
