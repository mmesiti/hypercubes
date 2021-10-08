#include "api_v2/tree_transform.hpp"
#include "utils/print_utils.hpp" // DEBUG
#include "utils/utils.hpp"
#include <algorithm>
#include <cmath>
#include <iostream> // DEBUG
#include <numeric>

namespace hypercubes {
namespace slow {
namespace internals {

KVTreePv2<bool> generate_nd_tree(std::vector<int> dimensions) {
  if (dimensions.size() > 0) {
    auto child = generate_nd_tree(tail(dimensions));
    decltype(KVTree<bool>::children) children;
    children.reserve(dimensions[0]);
    for (int i = 0; i < dimensions[0]; ++i)
      children.push_back({{i}, child});
    return mtkv(false, children);
  } else {
    return mtkv(true, {});
  }
}

KVTreePv2<bool> renumber_children_rec(const KVTreePv2<bool> t) {

  decltype(KVTree<bool>::children) children;
  auto tchildren = t->children;
  children.reserve(tchildren.size());
  for (auto i = 0; i != tchildren.size(); ++i) {
    children.push_back({{i}, renumber_children_rec(tchildren[i].second)});
  }
  return mtkv(t->n, children);
}

void _partition_children_into_subtrees(
    decltype(KVTree<bool>::children) &children, //
    const vector<int> &starts,                  //
    const vector<int> &ends,                    //
    const decltype(KVTree<bool>::children) &t_children) {

  children.reserve(ends.size());
  for (int child = 0; child < ends.size(); child++) {
    int i_grandchildren_start = starts[child];
    int i_grandchildren_end = ends[child];
    decltype(KVTree<bool>::children) grandchildren;
    grandchildren.reserve(i_grandchildren_end - i_grandchildren_start);

    for (auto i = i_grandchildren_start; i < i_grandchildren_end; ++i)
      grandchildren.push_back({{i}, //
                               renumber_children_rec(t_children[i].second)});

    children.push_back({{}, mtkv(false, grandchildren)});
  }
}

KVTreePv2<bool> q(KVTreePv2<bool> t, int level, int nparts) {
  decltype(KVTree<bool>::children) children;
  int size = t->children.size();
  if (level == 0) {
    float quotient = (float)size / nparts;
    vector<int> starts, ends;
    starts.reserve(nparts);
    ends.reserve(nparts);
    for (int child = 0; child < nparts; child++) {
      starts.push_back(std::round(child * quotient));
      ends.push_back(std::round((child + 1) * quotient));
    }
    _partition_children_into_subtrees(children, starts, ends, t->children);
    return mtkv(false, children);
  } else {
    return renumber_children(t, [level, nparts](auto subtree) {
      return q(subtree, level - 1, nparts);
    });
  }
}

KVTreePv2<bool> bb(KVTreePv2<bool> t, int level, int halosize) {
  decltype(KVTree<bool>::children) children;
  int size = t->children.size();
  if (level == 0) {
    std::vector<int> limits = {0,               //
                               halosize,        //
                               size - halosize, //
                               size};
    auto starts = limits; // except the last...
    auto ends = tail(limits);
    _partition_children_into_subtrees(children, starts, ends, t->children);
    return mtkv(false, children);
  } else {
    return renumber_children(t, [level, halosize](auto subtree) {
      return bb(subtree, level - 1, halosize);
    });
  }
}

KVTreePv2<bool> flatten(const KVTreePv2<bool> t, int levelstart, int levelend) {
  decltype(KVTree<bool>::children) children;
  if (levelstart == 0) {
    if (levelend > 1) {
      for (auto i = 0; i != t->children.size(); ++i) {
        auto tchild = flatten(t->children[i].second, 0, levelend - 1);
        for (auto grandchild : tchild->children) {
          auto keys = append(i, grandchild.first);
          children.push_back({keys, grandchild.second});
        }
      }
    } else {
      for (auto i = 0; i != t->children.size(); ++i) {
        auto tchild = t->children[i].second;
        children.push_back({{i}, tchild});
      }
    }
    return mtkv(false, children);
  } else {
    return renumber_children(t, //
                             [levelstart, levelend](auto subtree) {
                               return flatten(subtree, levelstart - 1,
                                              levelend - 1);
                             });
  }
}

KVTreePv2<bool> eo_naive(const KVTreePv2<bool> t, int level) {
  decltype(KVTree<bool>::children) children;
  if (level == 0) {
    children.reserve(2);
    decltype(children) E, O;
    decltype(children) EO[2] = {E, O};
    for (auto i = 0; i < t->children.size(); ++i) {
      auto keys = t->children[i].first;
      EO[std::accumulate(keys.begin(), keys.end(), 0) % 2].push_back(
          {{i}, t->children[i].second});
    }
    children.push_back({{0}, mtkv(false, EO[0])});
    children.push_back({{1}, mtkv(false, EO[1])});
    return mtkv(false, children);
  } else {
    return renumber_children(
        t, //
        [level](auto subtree) { return eo_naive(subtree, level - 1); });
  }
}

} // namespace internals

} // namespace slow
} // namespace hypercubes
