#include "size_tree_iterator.hpp"
#include "utils.hpp"
#include <cassert>

namespace hypercubes {

namespace slow {

using SizeTree = TreeP<std::pair<int, int>>;

Indices next(const SizeTree &size_tree, const Indices &idxs) {

  // function to retrieve idx
  // subtree->n.second;
  auto _get_start_idxs = [](SizeTree subtree) {
    return append(vtransform(tail(first_nodes_list(subtree)),
                             [](auto c) { return c.second; }),
                  0);
  };

  int s = size_tree->n.first;
  int idx = size_tree->n.second;
  int nidx = idxs[0];
  auto children = size_tree->children;
  vector<int> idx_children = vtransform(children,    //
                                        [](auto c) { //
                                          return c->n.second;
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
        return Indices{};
      else {
        new_sub_idxs = _get_start_idxs(children[i + 1]);
        // new_sub_idxs =
        //    append(vtransform(tail(first_nodes_list(children[i + 1])),
        //                      [](auto c) { return c.second; }),
        //           0);
        return append(idx_children[i + 1], new_sub_idxs);
      }
    } else
      return append(nidx, new_sub_idxs);
  }
}
} // namespace slow
} // namespace hypercubes
