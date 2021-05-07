#ifndef SIZE_TREE_ITERATOR_H_
#define SIZE_TREE_ITERATOR_H_
#include "geometry.hpp"
#include "tree.hpp"

namespace hypercubes {
namespace slow {

Indices next(const TreeP<std::pair<int, int>> &size_tree, const Indices &idxs);

}
} // namespace hypercubes
#endif // SIZE_TREE_ITERATOR_H_
