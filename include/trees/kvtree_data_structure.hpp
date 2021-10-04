#ifndef KVTREE_DATA_H_
#define KVTREE_DATA_H_
#include "tree_data_structure.hpp"
#include <algorithm>
#include <memory>
#include <sstream>
#include <utility>
#include <vector>

namespace hypercubes {
namespace slow {
namespace internals {

using std::pair;
using std::shared_ptr;
using std::vector;

template <class> struct KVTree;
template <class Node> using KVTreePv2 = shared_ptr<const KVTree<Node>>;

template <class Node> struct KVTree {
  Node n;
  vector<pair<int, KVTreePv2<Node>>> children;
  bool operator!=(const KVTree &other) const {
    if (n != other.n)
      return true;
    if (children.size() != other.children.size())
      return true;
    for (int i = 0; i < children.size(); ++i)
      if (children[i].first != other.children[i].first or //
          *children[i].second != *other.children[i].second)
        return true;
    return false;
  }
  bool operator==(const KVTree &other) const { return not(*this != other); };
};

template <class Node>
KVTreePv2<Node> mtkv(const Node n, const decltype(KVTree<Node>::children) v) {
  return std::make_shared<KVTree<Node>>(std::move(KVTree<Node>{n, v}));
}
template <class A> pair<int, A> mp(int n, A other) {
  return std::make_pair(n, other);
}

/* A newline before each subtree,
 * and a newline after each child. */
template <class Node> std::string kvtree_str(const KVTree<Node> &tree) {
  auto _tree_str = [&](const std::string &prefix, const KVTree<Node> &t,
                       auto f) -> std::string {
    std::stringstream ss;
    ss << t.n << std::endl; // head
    for (const auto &p : t.children) {
      auto c = p.second;
      int key = p.first;
      ss << prefix << key << ": " << f(prefix + "  ", *c, f);
    }
    auto res = ss.str();
    if (res[res.size() - 1] != '\n')
      res += '\n';
    return res;
  };
  return std::string("\n") + _tree_str("  ", tree, _tree_str);
}

// TODO: Memoise?
/* This function does the following transformation, from
 * (0, 1)
 *   (3, 4)
 *     (8, 80)
 *     (9, 80)
 *   (5, 4)
 *     (18, 80)
 *     (19, 80)
 *
 * to
 *
 * 1
 *   3: 4
 *     8: 80
 *     9: 80
 *   5: 4
 *     18: 80
 *     19: 80
 */
template <class Node>
KVTreePv2<Node> pull_keys_up(const TreeP<pair<int, Node>> t) {
  vector<pair<int, KVTreePv2<Node>>> children;
  children.reserve(t->children.size());
  std::transform(t->children.begin(), t->children.end(),
                 std::back_inserter(children), [](auto c) {
                   return mp(c->n.first, //
                             pull_keys_up(c));
                 });

  return mtkv(t->n.second, children);
}

// TODO: Memoise?
template <class Node>
TreeP<pair<int, Node>> push_keys_down(const KVTreePv2<Node> t, int key = 0) {
  vector<TreeP<pair<int, Node>>> children;
  children.reserve(t->children.size());
  std::transform(t->children.begin(), //
                 t->children.end(),   //
                 std::back_inserter(children),
                 [](auto c) { return push_keys_down(c.second, c.first); });

  return mt(mp(key, t->n), children);
}

} // namespace internals
} // namespace slow
} // namespace hypercubes

namespace std {

template <class Node>
std::ostream &
operator<<(std::ostream &os,
           const hypercubes::slow::internals::KVTree<Node> &tree) {
  os << hypercubes::slow::internals::kvtree_str(tree);
  return os;
}
} // namespace std

#endif // KVTREE_DATA_STRUCTURE_H_
