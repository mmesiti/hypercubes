#include "api_v2/tree_transform.hpp"
#include "utils/utils.hpp"
#include <algorithm>
#include <cmath>

namespace hypercubes {
namespace slow {
namespace internals {

KVTreePv2<bool> generate_nd_tree(std::vector<int> dimensions) {
  if (dimensions.size() > 0) {
    auto child = generate_nd_tree(tail(dimensions));
    decltype(KVTree<bool>::children) children;
    children.reserve(dimensions[0]);
    for (int i = 0; i < dimensions[0]; ++i)
      children.push_back({i, child});
    return mtkv(false, children);
  } else {
    return mtkv(true, {});
  }
}

KVTreePv2<bool> q(KVTreePv2<bool> t, int level, int nparts) {
  decltype(KVTree<bool>::children) children;
  int size = t->children.size();
  if (level == 0) {
    children.reserve(nparts);
    float quotient = (float)size / nparts;
    // creating the children
    for (int child = 0; child < nparts; child++) {
      int i_grandchildren_start = std::round(child * quotient);
      int i_grandchildren_end = std::round((child + 1) * quotient);
      decltype(children) grandchildren;
      std::transform(t->children.begin() + i_grandchildren_start,
                     t->children.begin() + i_grandchildren_end,
                     std::back_inserter(grandchildren),
                     [i_grandchildren_start](auto child) {
                       return mp(child.first - i_grandchildren_start,
                                 child.second);
                     });
      children.push_back({child, mtkv(false, grandchildren)});
    }
  } else {
    children.reserve(size);
    std::transform(t->children.begin(), //
                   t->children.end(),   //
                   std::back_inserter(children), [level, nparts](auto child) {
                     return mp(child.first, q(child.second, level - 1, nparts));
                   });
  }
  return mtkv(false, children);
}

KVTreePv2<bool> bb(KVTreePv2<bool> t, int level, int halosize) {
  decltype(KVTree<bool>::children) children;
  int size = t->children.size();
  if (level == 0) {
    children.reserve(3);
    std::vector<int> limits = {0,               //
                               halosize,        //
                               size - halosize, //
                               size};
    auto starts = limits; // except the last...
    auto ends = tail(limits);
    for (int child = 0; child < ends.size(); ++child) {
      int i_grandchildren_start = starts[child];
      int i_grandchildren_end = ends[child];
      decltype(children) grandchildren;
      std::transform(t->children.begin() + i_grandchildren_start,
                     t->children.begin() + i_grandchildren_end,
                     std::back_inserter(grandchildren),
                     [i_grandchildren_start](auto child) {
                       return mp(child.first - i_grandchildren_start,
                                 child.second);
                     }

      );
      children.push_back({child, mtkv(false, grandchildren)});
    }
  } else {
    children.reserve(size);
    std::transform(t->children.begin(), t->children.end(),
                   std::back_inserter(children), [level, halosize](auto child) {
                     return mp(child.first,
                               bb(child.second, level - 1, halosize));
                   });
  }
  return mtkv(false, children);
}

} // namespace internals

} // namespace slow
} // namespace hypercubes
