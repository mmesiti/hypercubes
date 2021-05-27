#ifndef TREE_DATA_H_
#define TREE_DATA_H_
#include <memory>
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
} // namespace slow
} // namespace hypercubes
#endif // TREE_DATA_H_
