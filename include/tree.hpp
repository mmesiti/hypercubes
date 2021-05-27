#ifndef TREE_H_
#define TREE_H_
#include "memoisation.hpp"
#include "print_utils.hpp"
#include "utils.hpp"
#include <algorithm>
#include <functional>
#include <map>
#include <ostream>
#include <sstream>
#include <stdexcept>

#include "tree_data_structure.hpp"

#include "memoisation_details.hpp"

namespace hypercubes {
namespace slow {

/* A newline before each subtree,
 * and a newline after each child. */
template <class Node> std::string tree_str(const Tree<Node> &tree) {
  auto _tree_str = [&](const std::string &prefix, const Tree<Node> &t,
                       auto f) -> std::string {
    std::stringstream ss;
    ss << std::endl << prefix << t.n; // head
    for (const auto &c : t.children)
      ss << f(prefix + "  ", *c, f) << std::endl;
    return ss.str();
  };
  return _tree_str("", tree, _tree_str);
}

/**
 * Get the full paths from the root
 *to every leaf of the tree, as a list
 **/
template <class Node>
vector<vector<Node>> get_all_paths(const TreeP<Node> &tree) {
  using ResType = vector<vector<Node>>;
  ResType res;
  if (tree->children.size() != 0)
    for (const TreeP<Node> &child : tree->children) {
      ResType child_results = get_all_paths(child);
      for (const vector<Node> &child_path : child_results) {
        vector<Node> path{tree->n};
        std::copy(child_path.begin(), //
                  child_path.end(),   //
                  std::back_inserter(path));
        res.push_back(path);
      }
    }
  else {
    res.push_back(vector<Node>{tree->n});
  }
  return res;
}

template <class Node>
vector<TreeP<Node>> get_flat_list_of_subtrees(const TreeP<Node> &tree,
                                              int level) {

  using ResType = vector<TreeP<Node>>;
  ResType res;
  if (tree->children.size() != 0 and level > 0) {
    for (const TreeP<Node> &c : tree->children) {
      ResType child_results = get_flat_list_of_subtrees(c, level - 1);
      std::copy(child_results.begin(), //
                child_results.end(),   //
                std::back_inserter(res));
    }
  } else {
    res.push_back(tree);
  }
  return res;
}

template <class Node> int get_max_depth(const TreeP<Node> &tree) {
  if (tree->children.size() == 0)
    return 1;
  else {
    int max = 0;
    for (const auto &c : tree->children) {
      int submax = get_max_depth(c);
      if (max < submax)
        max = submax;
    }
    return 1 + max;
  }
}

template <class Node>
TreeP<Node> memodetails::truncate_tree::base(                  //
    std::function<TreeP<Node>(const TreeP<Node> &, int)> frec, //
    const TreeP<Node> &tree,                                   //
    int level) {
  if (level > 1) {
    auto children = vtransform(tree->children,                 //
                               [level, &frec](TreeP<Node> c) { //
                                 return frec(c, level - 1);    //
                               });
    return mt(tree->n, children);

  } else if (level == 1) {
    return mt(tree->n, {});
  } else
    return TreeP<Node>(nullptr);
}

template <class Node>
TreeP<Node> truncate_treeM(const TreeP<Node> &tree, int level) {
  using namespace memodetails::truncate_tree;
  return Memo<Node>().memoised(tree, level);
}
template <class Node>
TreeP<Node> truncate_tree(const TreeP<Node> &tree, int level) {
  using namespace memodetails::truncate_tree;
  return Memo<Node>().nomemo(tree, level);
};

template <class Node>
vector<Node> memodetails::get_leaves_list::base(           //
    std::function<vector<Node>(const TreeP<Node> &)> frec, //
    const TreeP<Node> &tree) {
  vector<Node> res;
  if (tree->children.size() == 0)
    res.push_back(tree->n);
  else
    for (const auto &c : tree->children) {
      vector<Node> child_leaves = frec(c);
      std::copy(child_leaves.begin(), //
                child_leaves.end(),   //
                std::back_inserter(res));
    }
  return res;
}

template <class Node> vector<Node> get_leaves_listM(const TreeP<Node> &tree) {
  using namespace memodetails::get_leaves_list;
  return Memo<Node>().memoised(tree);
}
template <class Node> vector<Node> get_leaves_list(const TreeP<Node> &tree) {
  using namespace memodetails::get_leaves_list;
  return Memo<Node>().nomemo(tree);
}

template <class Node, class... Nodes>
TreeP<std::tuple<Node, Nodes...>> ziptree(const TreeP<Node> &t,
                                          const TreeP<Nodes> &...ts) {
  using ResType = TreeP<std::tuple<Node, Nodes...>>;
  auto n = std::make_tuple(t->n, ts->n...);
  vector<ResType> children;
  for (int i = 0; i < t->children.size(); ++i)
    children.push_back(ziptree(t->children[i], ts->children[i]...));
  return mt(n, children);
}

template <class Node, class NewNode, NewNode (*f)(Node)>
TreeP<NewNode> memodetails::nodemap::base(
    std::function<TreeP<NewNode>(const TreeP<Node> &)> frec, //
    const TreeP<Node> &t) {
  // NOTE: here resmap could be passed between calls.
  std::map<TreeP<Node>, TreeP<NewNode>> resmap;

  vector<TreeP<NewNode>> children =
      vtransform(t->children, //
                 [&resmap, &frec](const TreeP<Node> &c) {
                   if (resmap.find(c) == resmap.end())
                     resmap[c] = frec(c);
                   return resmap[c];
                 });
  return mt(f(t->n), children);
}

template <class Node, class NewNode, NewNode (*f)(Node)>
TreeP<NewNode> nodemapM(const TreeP<Node> &t) {
  using namespace memodetails::nodemap;
  return Memo<Node, NewNode, f>().memoised(t);
}

template <class Node, class NewNode, NewNode (*f)(Node)>
TreeP<NewNode> nodemap(const TreeP<Node> &t) {
  using namespace memodetails::nodemap;
  return Memo<Node, NewNode, f>().nomemo(t);
}

// TODO: consider memoisation
template <class Node, bool (*predicate)(Node)>
TreeP<Node> filternode(const TreeP<Node> &t) {

  vector<TreeP<Node>> selected_children;
  std::copy_if(t->children.begin(), t->children.end(), //
               std::back_inserter(selected_children),  //
               [](const TreeP<Node> &c) { return predicate(c->n); });

  vector<TreeP<Node>> pruned_children =
      vtransform(selected_children, [](const TreeP<Node> &c) {
        return filternode<Node, predicate>(c);
      });
  return mt(t->n, pruned_children);
}

template <class Node, class F>
TreeP<Node>                              //
_collapse_level(const TreeP<Node> &tree, //
                int level_to_collapse,   //
                int child_to_replace,    //
                F solder_subtree_in) {
  if (level_to_collapse == 0) {
    auto c = tree->children[child_to_replace];
    return solder_subtree_in(c, tree);
  } else {
    vector<TreeP<Node>> children =
        vtransform(tree->children, //
                   [level_to_collapse, child_to_replace,
                    &solder_subtree_in](const TreeP<Node> &c) {
                     return _collapse_level(c,                     //
                                            level_to_collapse - 1, //
                                            child_to_replace,      //
                                            solder_subtree_in);
                   });
    return mt(tree->n, children);
  }
}

// TODO: consider memoization.
template <class Node>
TreeP<Node>                             //
collapse_level(const TreeP<Node> &tree, //
               int level_to_collapse,   //
               int child_to_replace) {
  auto just_get_child = [](auto child, auto tree) { return child; };
  return _collapse_level(tree,              //
                         level_to_collapse, //
                         child_to_replace,  //
                         just_get_child);
}

template <class Node, class F1, class F2, class F3, class F4>
TreeP<Node> _bring_level_on_top(const TreeP<Node> &tree,              //
                                int level,                            //
                                F1 get_subtree_features,              //
                                F2 get_nchildren_from_other_features, //
                                F3 fix_tree_from_other_features,      //
                                F4 create_top_node) {
  /* Example:
   * _bring_level_on_top(t,1,...)
   *      a                b
   *    /   \           /  |  \
   *   b     b    ->   a   a   a
   * / | \ / | \      / \ / \ / \
   * d e f g h i      d g e h f i
   */
  // in example: nchildren_at_level = 3
  using TreeP = TreeP<Node>;
  TreeP top_tree = truncate_tree(tree, level + 2);
  vector<TreeP> subtrees = get_flat_list_of_subtrees(top_tree, level);
  /*    b
   *  / | \
   *  d e f
   *  ,
   *    b
   *  / | \
   *  g h i
   **/

  auto subtree_features = vtransform(subtrees, get_subtree_features);
  for (const auto &n_and_cs : subtree_features)
    if (subtree_features[0] != n_and_cs)
      throw std::invalid_argument("Not all subtrees are equivalent.");
  auto new_top = subtree_features[0].first;
  auto other_features = subtree_features[0].second;
  int nchildren_at_level = get_nchildren_from_other_features(other_features);

  vector<TreeP> new_subtrees;
  for (int child_to_pick = 0;              //
       child_to_pick < nchildren_at_level; //
       ++child_to_pick) {
    TreeP new_subtree = collapse_level(tree, level, child_to_pick);
    TreeP new_subtree_transformed = fix_tree_from_other_features(
        new_subtree, other_features, child_to_pick);

    new_subtrees.push_back(new_subtree_transformed);
  }
  /* new subtrees:
   *  a     a     a
   * / \ , / \ , / \
   * d g   e h   f i
   * */
  return mt(create_top_node(new_top), new_subtrees);
}

template <class Node>
TreeP<Node> //
bring_level_on_top(const TreeP<Node> &tree, int level) {
  using TreeP = TreeP<Node>;

  auto get_subtree_features = [](TreeP c) {
    return std::make_pair(c->n, c->children.size());
  };
  auto get_nchildren_from_other_features = [](int nchildren_at_level) {
    return nchildren_at_level;
  };
  auto fix_nothing = [](TreeP new_subtree,   //
                        int nchildren,       //
                        int child_to_pick) { //
    return new_subtree;
  };
  auto create_top_node = [](Node new_top) { return new_top; };

  return _bring_level_on_top(tree,                              //
                             level,                             //
                             get_subtree_features,              //
                             get_nchildren_from_other_features, //
                             fix_nothing,                       //
                             create_top_node);
}

template <class Node, class F>
TreeP<Node> _swap_levels(const TreeP<Node> &tree,
                         const vector<int> &new_level_ordering,
                         F fix_new_tree) {

  using TreeP = TreeP<Node>;
  if (new_level_ordering.size() == 0)
    return tree;

  int next_level = new_level_ordering[0];
  TreeP new_tree = bring_level_on_top(tree, next_level);
  TreeP new_tree_transformed = fix_new_tree(new_tree, tree);

  vector<int> sub_new_level_ordering = [](const vector<int> &level_ordering) {
    vector<int> res;
    int lvl_removed = level_ordering[0];
    for (int i = 1; i < level_ordering.size(); ++i) {
      int l = level_ordering[i];
      if (l > lvl_removed)
        res.push_back(l - 1);
      else
        res.push_back(l);
    }
    return res;
  }(new_level_ordering);

  vector<TreeP> new_children =
      vtransform(new_tree_transformed->children, //
                 [&sub_new_level_ordering, &fix_new_tree](const TreeP &c) {
                   return _swap_levels(c, sub_new_level_ordering, fix_new_tree);
                 });
  return mt(new_tree_transformed->n, new_children);
}

template <class Node>
TreeP<Node>                          //
swap_levels(const TreeP<Node> &tree, //
            const vector<int> &new_level_ordering) {
  auto no_changes = [](const TreeP<Node> &new_tree, //
                       const TreeP<Node> &parent_tree) { return new_tree; };
  return _swap_levels(tree, new_level_ordering, no_changes);
}

template <class Node> vector<Node> first_nodes_list(const TreeP<Node> &tree) {

  vector<Node> res;
  auto _first_nodes_list = [](const TreeP<Node> subtree, vector<Node> &r,
                              auto &f) -> void {
    r.push_back(subtree->n);
    if (subtree->children.size() != 0)
      f(subtree->children[0], r, f);
  };

  _first_nodes_list(tree, res, _first_nodes_list);
  return res;
}

template <class Node>
vector<Node> depth_first_flatten(const TreeP<Node> &tree) {

  vector<Node> res;
  auto _depth_first_flatten = [](const TreeP<Node> &subtree, vector<Node> &r,
                                 auto &f) -> void {
    r.push_back(subtree->n);
    for (const auto &c : subtree->children)
      f(c, r, f);
  };

  _depth_first_flatten(tree, res, _depth_first_flatten);
  return res;
}

} // namespace slow
} // namespace hypercubes

namespace std {

template <class Node>
std::ostream &operator<<(std::ostream &os,
                         const hypercubes::slow::Tree<Node> &tree) {
  os << hypercubes::slow::tree_str(tree);
  return os;
}

} // namespace std
#endif // TREE_H_
