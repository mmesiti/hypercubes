#include "api_v2/tree_transform.hpp"
#include "utils/utils.hpp"
#include <cmath>

namespace hypercubes {
namespace slow {
namespace internals {

TreeP<bool> generate_nd_tree(std::vector<int> dimensions) {
  if (dimensions.size() > 0) {
    auto child = generate_nd_tree(tail(dimensions));
    std::vector<TreeP<bool>> children(dimensions[0], child);
    return mt(false, children);
  } else {
    return mt(true, {});
  }
}

TreeP<bool> q(TreeP<bool> t, int level, int nparts) {
  std::vector<TreeP<bool>> children;
  int size = t->children.size();
  if (level == 0) {
    children.reserve(nparts);
    float quotient = (float)size / nparts;
    // creating the children
    for (int child = 0; child < nparts; child++) {
      int i_grandchildren_start = std::round(child * quotient);
      int i_grandchildren_end = std::round((child + 1) * quotient);
      std::vector<TreeP<bool>> grandchildren;
      std::copy(t->children.begin() + i_grandchildren_start,
                t->children.begin() + i_grandchildren_end,
                std::back_inserter(grandchildren));
      children.push_back(mt(false, grandchildren));
    }
  } else {
    children.reserve(size);
    std::transform(t->children.begin(), t->children.end(),
                   std::back_inserter(children),
                   [level, nparts](TreeP<bool> child) {
                     return q(child, level - 1, nparts);
                   });
  }
  return mt(false, children);
}

TreeP<bool> bb(TreeP<bool> t, int level, int halosize) {
  std::vector<TreeP<bool>> children;
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
      std::vector<TreeP<bool>> grandchildren;
      std::copy(t->children.begin() + i_grandchildren_start,
                t->children.begin() + i_grandchildren_end,
                std::back_inserter(grandchildren));
      children.push_back(mt(false, grandchildren));
    }
  } else {
    children.reserve(size);
    std::transform(t->children.begin(), t->children.end(),
                   std::back_inserter(children),
                   [level, halosize](TreeP<bool> child) {
                     return bb(child, level - 1, halosize);
                   });
  }
  return mt(false, children);
}

} // namespace internals

} // namespace slow
} // namespace hypercubes
