#ifndef TREE_TRANSFORM_H_
#define TREE_TRANSFORM_H_
#include "trees/kvtree_data_structure.hpp"
#include <vector>

namespace hypercubes {
namespace slow {
namespace internals {

/** Generates a very simple tree
 * representing a N-dimensional array
 * from a list of dimensions.
 * */
KVTreePv2<bool> generate_nd_tree(std::vector<int> Dimensions);
/** Splits a level of a tree in n-parts,
 * making sure the partitions are as equal as possible.
 * */
KVTreePv2<bool> q(KVTreePv2<bool>, int level, int nparts);
KVTreePv2<bool> bb(KVTreePv2<bool>, int level, int nparts);

} // namespace internals

} // namespace slow
} // namespace hypercubes

#endif // TREE_TRANSFORM_H_
