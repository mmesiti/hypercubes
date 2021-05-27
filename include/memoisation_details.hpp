#ifndef MEMOISATION_DETAILS_H_
#define MEMOISATION_DETAILS_H_
#include "memoisation.hpp"
#include "tree_data_structure.hpp"
#include "utils.hpp"

namespace hypercubes {
namespace slow {

namespace truncate_tree_detail {
// forward declaration
template <class Node>
TreeP<Node> base(                                              //
    std::function<TreeP<Node>(const TreeP<Node> &, int)> frec, //
    const TreeP<Node> &tree,                                   //
    int level);

// boilerplate
template <class Node>
class _Memoiser : public Memoiser<TreeP<Node>, TreeP<Node>, int> {
public:
  _Memoiser() : Memoiser<TreeP<Node>, TreeP<Node>, int>(base<Node>){};
};

template <class Node> TreeP<Node> memoised(const TreeP<Node> &tree, int level) {
  _Memoiser<Node> m;
  return m(tree, level);
}

template <class Node> TreeP<Node> nomemo(const TreeP<Node> &tree, int level) {
  return base<Node>(nomemo<Node>, tree, level);
}
} // namespace truncate_tree_detail

namespace get_leaves_list_detail {

// forward declaration
template <class Node>
vector<Node> base(                                         //
    std::function<vector<Node>(const TreeP<Node> &)> frec, //
    const TreeP<Node> &tree);

// boilerplate
template <class Node>
class _Memoiser : public Memoiser<vector<Node>, TreeP<Node>> {
public:
  _Memoiser() : Memoiser<vector<Node>, TreeP<Node>>(base<Node>){};
};

template <class Node> vector<Node> memoised(const TreeP<Node> &tree) {
  _Memoiser<Node> m;
  return m(tree);
}

template <class Node> vector<Node> nomemo(const TreeP<Node> &tree) {
  return base<Node>(nomemo<Node>, tree);
}
} // namespace get_leaves_list_detail

namespace nodemap_detail {

// forward declaration
template <class Node, class NewNode, NewNode (*f)(Node)>
TreeP<NewNode> base(std::function<TreeP<NewNode>(const TreeP<Node> &)> frec, //
                    const TreeP<Node> &t);

// boilerplate

template <class Node, class NewNode, NewNode (*f)(Node)>
class _Memoiser : public Memoiser<TreeP<NewNode>, TreeP<Node>> {
public:
  _Memoiser() : Memoiser<TreeP<NewNode>, TreeP<Node>>(base<Node, NewNode, f>){};
};

template <class Node, class NewNode, NewNode (*f)(Node)>
TreeP<NewNode> memoised(const TreeP<Node> &t) {
  _Memoiser<Node, NewNode, f> m;
  return m(t);
}

template <class Node, class NewNode, NewNode (*f)(Node)>
TreeP<NewNode> nomemo(const TreeP<Node> &t) {
  return base<Node, NewNode, f>(nomemo<Node, NewNode, f>, t);
}

} // namespace nodemap_detail

namespace number_children_detail {

template <class Value> using Out = TreeP<std::pair<int, Value>>;

// forward declaration
template <class Value>
Out<Value> base(                                          //
    std::function<Out<Value>(const TreeP<Value> &)> frec, //
    const TreeP<Value> &tree);

// boilerplate
template <class Value>
class _Memoiser : public Memoiser<Out<Value>, TreeP<Value>> {
public:
  _Memoiser() : Memoiser<Out<Value>, TreeP<Value>>(base<Value>){};
};

template <class Value> Out<Value> memoised(const TreeP<Value> &tree) {
  _Memoiser<Value> m;
  return m(tree);
}
template <class Value> Out<Value> nomemo(const TreeP<Value> &tree) {
  return base<Value>(nomemo<Value>, tree);
}
} // namespace number_children_detail

namespace prune_tree_details {

template <class Value> using Tree = TreeP<std::pair<int, Value>>;
using Predicate = std::function<bool(vector<int>)>;
using Indices = vector<int>;

// forward declaration
template <class Value>
Tree<Value> base_wp(
    std::function<Tree<Value>(const Tree<Value> &, const Indices &)> frec, //
    const Tree<Value> &t,                                                  //
    const Indices &top_idxs,                                               //
    Predicate predicate);

// boilerplate

template <class Value>
class _Memoiser : public Memoiser<Tree<Value>, Tree<Value>, Indices> {
public:
  _Memoiser(Predicate predicate)
      : Memoiser<Tree<Value>, Tree<Value>, Indices>(
            [&predicate](std::function<Tree<Value>(const Tree<Value> &, //
                                                   const Indices &)>
                             frec,             //
                         const Tree<Value> &t, //
                         const Indices &top_idxs) {
              return base_wp<Value>(frec, t, top_idxs, predicate);
            }){};
};
template <class Value>
Tree<Value> memoised(const Tree<Value> &tree, const Indices &top_idxs,
                     Predicate predicate) {
  _Memoiser<Value> m(predicate);
  return m(tree, top_idxs);
}
template <class Value>
Tree<Value> nomemo(const Tree<Value> &t,    //
                   const Indices &top_idxs, //
                   Predicate predicate) {

  return base_wp<Value>(
      [&predicate](auto tree, auto top_idxs) -> Tree<Value> {
        return nomemo<Value>(tree, top_idxs, predicate);
      },
      t, top_idxs, predicate);
}

} // namespace prune_tree_details
} // namespace slow
} // namespace hypercubes

#endif // MEMOISATION_DETAILS_H_
