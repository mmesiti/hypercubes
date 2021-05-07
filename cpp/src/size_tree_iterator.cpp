#include "size_tree_iterator.hpp"

namespace hypercubes {

namespace slow {

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
  auto children = size_tree->children;
  int nidx = idxs[0];

  if (children.size() == 0) {
    int new_idx = nidx + 1;
    if (new_idx == s)
      return Indices{};
    else
      return Indices{new_idx};

  } else {
    int i;
    SizeTree c;
    for (i = 0; i < children.size(); ++i) {
      if (children[i]->n.second == nidx) {
        c = children[i];
        break;
      }
      Indices sub_idxs;
      std::copy(idxs.begin() + 1, idxs.end(), std::back_inserter(sub_idxs));
      Indices new_sub_idxs = next(c, sub_idxs);
      int new_nidx;
      if (new_sub_idxs.size() == 0) {
        if (i == children.size() - 1)
          return Indices{};
        else {
          new_sub_idxs = get_start_idxs(children[i + 1]);
          new_nidx = children[i + 1]->n.second;
        }
      } else { //
        Indices res{nidx};
        std::copy(new_sub_idxs.begin(), //
                  new_sub_idxs.end(),   //
                  std::back_inserter(res));
        return res;
      }
    }
  }
} // namespace slow
} // namespace hypercubes
