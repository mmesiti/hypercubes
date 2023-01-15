#ifndef KVTREE_V2_H_
#define KVTREE_V2_H_
#include "exceptions/exceptions.hpp"
#include "kvtree_data_structure.hpp"
#include <set>

namespace hypercubes {
namespace slow {
namespace internals {

// TODO: Remove?
template <class Value>
const KVTreePv2<Value>                                //
collapse_level_by_index(const KVTreePv2<Value> &tree, //
                        int level_to_collapse,        //
                        int child_index_to_replace) {
  if (level_to_collapse == 0) {
    return tree->children[child_index_to_replace].second;
  } else {
    decltype(KVTree<Value>::children) children;
    children.reserve(tree->children.size());
    for (auto ktree : tree->children) {
      const vector<int> k = ktree.first;
      KVTreePv2<Value> v = collapse_level_by_index(
          ktree.second, level_to_collapse - 1, child_index_to_replace);
      children.push_back({k, v});
    }
    return mtkv(tree->n, children);
  }
}

// TODO: Consider some kind of memoisation
//       (e.g., skipping subtrees that have already been looked at)
template <class Value>
void _collect_keys_at_level(std::set<vector<int>> &out,   //
                            const KVTreePv2<Value> &tree, //
                            int level) {
  if (level == 0) {
    for (auto &c : tree->children)
      out.insert(c.first);
  } else {
    for (auto &c : tree->children)
      _collect_keys_at_level(out, c.second, level - 1);
  }
}
// TODO: Consider some kind of memoisation
//       (e.g., skipping subtrees that have already been looked at)
template <class Value>
void _collect_nodes_at_level(std::set<Value> &nodes,       //
                             const KVTreePv2<Value> &tree, //
                             int level) {
  if (level == 0) {
    nodes.insert(tree->n);
  } else {
    for (auto &c : tree->children)
      _collect_nodes_at_level(nodes, c.second, level - 1);
  }
}

template <class Value>
void _throw_different_nodes_error(const std::set<Value> &nodes,
                                  const std::string &other_info) {
  std::stringstream message;

  switch (nodes.size()) {
  case 0:
    message << other_info << " No nodes found." << std::endl;
    break;
  case 1:
    // all ok
    return;
  default:
    message << "Not all nodes are equivalent:" << std::endl;
    for (auto &n : nodes) {
      message << n << ", ";
    }
    message << std::endl;
  }
  throw TreeLevelPermutationError(message.str().c_str());
}

template <class Value>
int get_depth(const KVTreePv2<Value> &tree, Value leaf_value) {
  if (tree->n == leaf_value)
    return 0;
  else
    return get_depth(tree->children[0].second, leaf_value) + 1;
}

} // namespace internals
} // namespace slow
} // namespace hypercubes

#endif // KVTREE_V2_H_
