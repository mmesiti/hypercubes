#ifndef TREE_DATA_H_
#define TREE_DATA_H_
#include <memory>
#include <sstream>
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
/* A newline before each subtree,
 * and a newline after each child. */
template <class Node> std::string tree_str(const Tree<Node> &tree) {
  auto _tree_str = [&](const std::string &prefix, const Tree<Node> &t,
                       auto f) -> std::string {
    std::stringstream ss;
    ss << t.n << std::endl; // head
    for (const auto &c : t.children)
      ss << prefix << f(prefix + "  ", *c, f);
    auto res = ss.str();
    if (res[res.size() - 1] != '\n')
      res += '\n';
    return res;
  };
  return std::string("\n") + _tree_str("  ", tree, _tree_str);
}

} // namespace internals
} // namespace slow
} // namespace hypercubes
namespace std {

template <class Node>
std::ostream &operator<<(std::ostream &os,
                         const hypercubes::slow::internals::Tree<Node> &tree) {
  os << hypercubes::slow::internals::tree_str(tree);
  return os;
}

} // namespace std

#endif // TREE_DATA_H_
