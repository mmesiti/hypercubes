#ifndef TREE_H_
#define TREE_H_
#include "utils.hpp"
#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <tuple>
#include <vector>

namespace hypercubes {
namespace slow {

using std::shared_ptr;
using std::vector;

template <class> struct Tree;
template <class Node> using TreeP = shared_ptr<Tree<Node>>;

template <class Node> struct Tree {
  Node n;
  vector<TreeP<Node>> children;
  bool operator!=(const Tree &other) const {
    if (n != other.n)
      return true;
    if (children.size() != other.children.size())
      return true;
    for (int i = 0; i < children.size(); ++i)
      if (*children[i] != *other.children[i])
        return true;
    return false;
  }
  bool operator==(const Tree &other) const { return not(*this != other); };
  bool operator<(const Tree &other) {
    if (n < other.n)
      return true;
    if (other.n < n)
      return false;
    for (int i = 0;                                         //
         i < children.size() and i < other.children.size(); //
         ++i)
      if (*children[i] < *other.children[i])
        return true;
    if (children.size() < other.children.size())
      return true;
    return false;
  }
};

template <class Node> struct less {

  bool operator()(const TreeP<Node> &t1, //
                  const TreeP<Node> &t2) {
    return *t1 < *t2;
  }
  bool operator()(const Tree<Node> &t1, //
                  const Tree<Node> &t2) {
    return t1 < t2;
  }
};

template <class Node>
TreeP<Node> mt(const Node n, const vector<TreeP<Node>> v) {
  return std::make_shared<Tree<Node>>(std::move(Tree<Node>{n, v}));
}

/* A newline before each subtree,
 * and a newline after each child. */
template <class Node> std::string tree_str(const Tree<Node> &tree) {
  std::function<std::string(const std::string &, //
                            const Tree<Node> &)>
      _tree_str;
  _tree_str = [&](const std::string &prefix,
                  const Tree<Node> &t) -> std::string {
    std::stringstream ss;
    ss << std::endl << prefix << t.n; // head
    for (const auto &c : t.children)
      ss << _tree_str(prefix + "  ", *c) << std::endl;
    return ss.str();
  };
  return _tree_str("", tree);
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
/**
 * */

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

// TODO: Consider memoization.
template <class Node>
TreeP<Node> truncate_tree(const TreeP<Node> &tree, int level) {
  if (level > 1) {
    auto children = vtransform(tree->children,                       //
                               [level](TreeP<Node> c) {              //
                                 return truncate_tree(c, level - 1); // FIXED
                               });
    return mt(tree->n, children);

  } else if (level == 1) {
    return mt(tree->n, {});
  } else
    return TreeP<Node>(nullptr);
}

template <class Node> vector<Node> get_leaves_list(const TreeP<Node> &tree) {
  vector<Node> res;
  if (tree->children.size() == 0)
    res.push_back(tree->n);
  else
    for (const auto &c : tree->children) {
      vector<Node> child_leaves = get_leaves_list(c);
      std::copy(child_leaves.begin(), //
                child_leaves.end(),   //
                std::back_inserter(res));
    }
  return res;
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

// TODO: consider memoization.
template <class Node, class NewNode>
TreeP<NewNode> nodemap(const TreeP<Node> &t, std::function<NewNode(Node)> f) {
  // NOTE: here resmap could be passed between calls.
  std::map<TreeP<Node>, TreeP<NewNode>> resmap;

  vector<TreeP<NewNode>> children =
      vtransform(t->children, //
                 [&f, &resmap](const TreeP<Node> &c) {
                   if (resmap.find(c) == resmap.end())
                     resmap[c] = nodemap(c, f);
                   return resmap[c];
                 });
  return mt(f(t->n), children);
}

// TODO: consider memoization.
template <class Node>
TreeP<Node> collapse_level(const TreeP<Node> &tree, int level_to_collapse,
                           int child_to_replace) {
  if (level_to_collapse == 0)
    return tree->children[child_to_replace];
  else {
    vector<TreeP<Node>> children = vtransform(
        tree->children, //
        [level_to_collapse, child_to_replace](const TreeP<Node> &c) {
          return collapse_level(c, level_to_collapse - 1, child_to_replace);
        });
    return mt(tree->n, children);
  }
}

template <class Node>
TreeP<Node> bring_level_on_top(const TreeP<Node> &tree, int level) {
  /* Example:
   * bring_level_on_top(t,1)
   *      a                b
   *    /   \           /  |  \
   *   b     b    ->   a   a   a
   * / | \ / | \      / \ / \ / \
   * d e f g h i      d g e h f i
   */

  // in example: nchildren_at_level = 3
  using TreeP = TreeP<Node>;
  int nchildren_at_level = [&tree, level]() {
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
    int nchildren_at_level = get_leaves_list(subtrees[0]).size();
    for (const auto &st : subtrees)
      if (nchildren_at_level != get_leaves_list(st).size())
        throw std::invalid_argument(
            "Not all subtrees have the same number of children.");
    return nchildren_at_level;
  }();

  Node new_top = [&tree, level]() {
    TreeP top_tree = truncate_tree(tree, level + 1);
    /*
     *      a
     *    /   \
     *   b     b
     */
    vector<Node> nodes_at_level = get_leaves_list(top_tree);
    /* b,  b */
    for (const auto &l : nodes_at_level)
      if (l != nodes_at_level[0])
        throw std::invalid_argument(
            "Not all nodes are equal on the required level.");
    return nodes_at_level[0];
  }();

  vector<TreeP> new_subtrees;
  for (int child_to_pick = 0;              //
       child_to_pick < nchildren_at_level; //
       ++child_to_pick)
    new_subtrees.push_back(collapse_level(tree, level, child_to_pick));
  /* new subtrees:
   *  a     a     a
   * / \ , / \ , / \
   * d g   e h   f i
   * */
  return mt(new_top, new_subtrees);
}

template <class Node>
TreeP<Node> swap_levels(const TreeP<Node> &tree,
                        const vector<int> &new_level_ordering) {

  if (new_level_ordering.size() == 0)
    return tree;

  int next_level = new_level_ordering[0];
  TreeP<Node> new_tree = bring_level_on_top(tree, next_level);

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

  vector<TreeP<Node>> new_children =
      vtransform(new_tree->children, //
                 [&sub_new_level_ordering](const TreeP<Node> &c) {
                   return swap_levels(c, sub_new_level_ordering);
                 });
  return mt(new_tree->n, new_children);
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
