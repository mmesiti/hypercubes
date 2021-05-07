#include "size_tree_iterator.hpp"
#include <cassert>

namespace hypercubes {

namespace slow {

Indices tail(Indices idx) {
  Indices res;
  std::copy(idx.begin() + 1, idx.end(), std::back_inserter(res));
  return res;
}
Indices append(int x, Indices idx) {
  Indices res{x};
  std::copy(idx.begin(), idx.end(), std::back_inserter(res));
  return res;
}

using SizeTree = TreeP<std::pair<int, int>>;
Indices next(const SizeTree &size_tree, const Indices &idxs) {

  // function to retrieve idx
  // subtree->n.second;
  std::function<Indices(SizeTree)> get_start_idxs =
      [&get_start_idxs](SizeTree subtree) -> Indices {
    auto children = subtree->children;
    if (children.size() == 0)
      return Indices{0};
    else {
      Indices res{children[0]->n.second};
      for (auto c : get_start_idxs(children[0]))
        res.push_back(c);
      return res;
    }
  };

  int s = size_tree->n.first;
  int idx = size_tree->n.second;
  int nidx = idxs[0];
  auto children = size_tree->children;
  vector<int> idx_children;
  std::transform(children.begin(),                 //
                 children.end(),                   //
                 std::back_inserter(idx_children), //
                 [](auto c) { return c->n.second; });

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
        new_sub_idxs = get_start_idxs(children[i + 1]);
        return append(idx_children[i + 1], new_sub_idxs);
      }
    } else
      return append(nidx, new_sub_idxs);
  }
}
} // namespace slow
} // namespace hypercubes
