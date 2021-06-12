#ifndef TREE_DATA_H_
#define TREE_DATA_H_
#include <memory>
#include <tuple>
#include <vector>

namespace hypercubes {
namespace slow {
namespace internals {

using std::shared_ptr;
using std::vector;

template <class> struct Tree;
template <class Node> using TreeP = shared_ptr<const Tree<Node>>;

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

template <class Node>
TreeP<Node> mt(const Node n, const vector<TreeP<Node>> v) {
  return std::make_shared<Tree<Node>>(std::move(Tree<Node>{n, v}));
}
} // namespace internals
} // namespace slow
} // namespace hypercubes
#endif // TREE_DATA_H_
