#ifndef __PARTITION_ORDERING_H_
#define __PARTITION_ORDERING_H_

#include "toposorting.hpp"
#include <algorithm>
#include <iterator>
#include <vector>

namespace hypercubes {
namespace slow {
namespace multiDordering {

using P = toposorting::SortablePartitioning::P;

struct Node {
  using Nodep = std::shared_ptr<Node>;

  P n;
  std::vector<Nodep> children;

  Node(P n_) : n(n_) {
    auto n_children = n_->children();
    std::transform(n_children.begin(),           //
                   n_children.end(),             //
                   std::back_inserter(children), //
                   [](auto x) { return Node(x); });
  };
};

} // namespace multiDordering
} // namespace slow
} // namespace hypercubes

#endif // __PARTITION_TREE_H_
