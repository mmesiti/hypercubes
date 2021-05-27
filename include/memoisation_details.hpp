#ifndef MEMOISATION_DETAILS_H_
#define MEMOISATION_DETAILS_H_
#include "memoisation.hpp"
#include "partitioners.hpp"
#include "tree_data_structure.hpp"
#include "utils.hpp"

namespace hypercubes {
namespace slow {
namespace memodetails {

namespace truncate_tree {
// forward declaration
template <class Node>
TreeP<Node> base(                                              //
    std::function<TreeP<Node>(const TreeP<Node> &, int)> frec, //
    const TreeP<Node> &tree,                                   //
    int level);

// boilerplate
template <class Node>
class Memo : public Memoiser<TreeP<Node>, TreeP<Node>, int> {
public:
  Memo() : Memoiser<TreeP<Node>, TreeP<Node>, int>(base<Node>){};
};

} // namespace truncate_tree

namespace get_leaves_list {

// forward declaration
template <class Node>
vector<Node> base(                                         //
    std::function<vector<Node>(const TreeP<Node> &)> frec, //
    const TreeP<Node> &tree);

// boilerplate
template <class Node> class Memo : public Memoiser<vector<Node>, TreeP<Node>> {
public:
  Memo() : Memoiser<vector<Node>, TreeP<Node>>(base<Node>){};
};

} // namespace get_leaves_list

namespace nodemap {

// forward declaration
template <class Node, class NewNode, NewNode (*f)(Node)>
TreeP<NewNode> base(std::function<TreeP<NewNode>(const TreeP<Node> &)> frec, //
                    const TreeP<Node> &t);

// boilerplate

template <class Node, class NewNode, NewNode (*f)(Node)>
class Memo : public Memoiser<TreeP<NewNode>, TreeP<Node>> {
public:
  Memo() : Memoiser<TreeP<NewNode>, TreeP<Node>>(base<Node, NewNode, f>){};
};

} // namespace nodemap

namespace number_children {

template <class Value> using Out = TreeP<std::pair<int, Value>>;

// forward declaration
template <class Value>
Out<Value> base(                                          //
    std::function<Out<Value>(const TreeP<Value> &)> frec, //
    const TreeP<Value> &tree);

// boilerplate
template <class Value> class Memo : public Memoiser<Out<Value>, TreeP<Value>> {
public:
  Memo() : Memoiser<Out<Value>, TreeP<Value>>(base<Value>){};
};

} // namespace number_children

namespace prune_tree {

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
class Memo : public Memoiser<Tree<Value>, Tree<Value>, Indices> {
public:
  Memo(Predicate predicate)
      : Memoiser<Tree<Value>, Tree<Value>, Indices>(
            [&predicate](std::function<Tree<Value>(const Tree<Value> &, //
                                                   const Indices &)>
                             frec,             //
                         const Tree<Value> &t, //
                         const Indices &top_idxs) {
              return base_wp<Value>(frec, t, top_idxs, predicate);
            }){};
};
} // namespace prune_tree

namespace get_partition_tree {
using partitioners::PartList;
using PartitionTree = TreeP<std::shared_ptr<IPartitioning>>;
PartitionTree
base(std::function<PartitionTree(SizeParityD, const PartList &)> frec, //
     SizeParityD spd,                                                  //
     const PartList &partitioners);

class Memo : public Memoiser<PartitionTree, SizeParityD, PartList> {
public:
  Memo() : Memoiser<PartitionTree, SizeParityD, PartList>(base) {}
};

} // namespace get_partition_tree

} // namespace memodetails
} // namespace slow
} // namespace hypercubes

#endif // MEMOISATIONS_H_
