#ifndef TREE_H_
#define TREE_H_
#include <algorithm>
#include <functional>
#include <memory>
#include <ostream>
#include <sstream>
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
};

template <class Node> TreeP<Node> mt(Node n, vector<TreeP<Node>> v) {
  return std::make_shared<Tree<Node>>(std::move(Tree<Node>{n, v}));
}

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
template <class Node> vector<vector<Node>> get_all_paths(TreeP<Node> tree) {
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
vector<TreeP<Node>> get_flat_list_of_subtrees(TreeP<Node> tree, int level) {

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

template <class Node> int get_max_depth(TreeP<Node> tree) {
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

template <class Node> TreeP<Node> truncate_tree(TreeP<Node> tree, int level) {
  if (level > 1) {
    vector<TreeP<Node>> children;
    std::transform(tree->children.begin(), tree->children.end(),
                   std::back_inserter(children), [level](TreeP<Node> c) {
                     return truncate_tree(c, level - 1);
                   });
    return mt(tree->n, children);

  } else {
    return mt(tree->n, {});
  }
}

template <class Node> vector<Node> get_leaves_list(TreeP<Node> tree) {
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
TreeP<std::tuple<Node, Nodes...>> ziptree(TreeP<Node> t, TreeP<Nodes>... ts) {
  using ResType = TreeP<std::tuple<Node, Nodes...>>;
  auto n = std::make_tuple(t->n, ts->n...);
  vector<ResType> children;
  for (int i = 0; i < t->children.size(); ++i)
    children.push_back(ziptree(t->children[i], ts->children[i]...));
  return mt(n, children);
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
