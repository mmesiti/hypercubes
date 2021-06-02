#include "trees/size_tree_iterator.hpp"
#include "utils/utils.hpp"
#include <cassert>

namespace hypercubes {
namespace slow {
namespace internals {

Indices get_start_idxs(const SizeTree &sizetree) {
  return append(vtransform(tail(first_nodes_list(sizetree)),
                           [](auto c) { return c.first; }),
                0);
}

Indices next(const SizeTree &size_tree, const Indices &idxs) {

  // function to retrieve idx
  // subtree->n.first;

  int s = size_tree->n.second;
  int idx = size_tree->n.first;
  int nidx = idxs[0];
  auto children = size_tree->children;
  vector<int> idx_children = vtransform(children,    //
                                        [](auto c) { //
                                          return c->n.first;
                                        });

  if (children.size() == 0) {
    // leaf case.
    // Children are sites in the partition
    // they are all present - no holes
    int new_nidx = nidx + 1;
    if (new_nidx == s) // last site in the partition
      return Indices{};
    else
      return Indices{new_nidx};
  } else {
    int i = std::find(idx_children.begin(), //
                      idx_children.end(),   //
                      nidx) -
            idx_children.begin();
    assert(i != idx_children.size());
    Indices sub_idxs = tail(idxs);
    Indices new_sub_idxs = next(children[i], sub_idxs);
    if (new_sub_idxs.size() == 0) {
      if (nidx == *idx_children.rbegin())
        return Indices{}; // last in subtree
      else {
        new_sub_idxs = get_start_idxs(children[i + 1]);
        return append(idx_children[i + 1], new_sub_idxs);
      }
    } else
      return append(nidx, new_sub_idxs);
  }
}

Indices get_end_idxs() { return Indices{}; }

} // namespace internals
} // namespace slow
} // namespace hypercubes
