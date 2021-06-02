#ifndef SIZE_TREE_ITERATOR_H_
#define SIZE_TREE_ITERATOR_H_
#include "geometry/geometry.hpp"
#include "tree.hpp"

namespace hypercubes {
namespace slow {
namespace internals {

using SizeTree = TreeP<std::pair<int, int>>;
Indices get_start_idxs(const SizeTree &sizetree);
Indices next(const TreeP<std::pair<int, int>> &size_tree, const Indices &idxs);
Indices get_end_idxs();

} // namespace internals
} // namespace slow
} // namespace hypercubes
#endif // SIZE_TREE_ITERATOR_H_
