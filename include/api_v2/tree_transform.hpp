#ifndef TREE_TRANSFORM_H_
#define TREE_TRANSFORM_H_
#include "exceptions/exceptions.hpp"
#include "trees/kvtree_data_structure.hpp"
#include "utils/utils.hpp"
#include <cassert>
#include <vector>

namespace hypercubes {
namespace slow {
namespace internals {

/** Generates a very simple tree
 * representing a N-dimensional array
 * from a list of dimensions.
 * It is also guaranteed yield an identity transformation
 * when used with index_pullback.
 * */
KVTreePv2<bool> generate_nd_tree(std::vector<int> Dimensions);

/** Level splitting functions.
 * The new level uses {} for all the keys
 * as there is no correspondent level in the input tree.
 * NOTE: the subtrees at depth level+1 are untouched
 *       and concide with the relative subtrees in the input tree.
 *       This means that they should be ignored by index_pullback. */

/** Splits a level of a tree in n-parts,
 * making sure the partitions are as equal as possible. */
KVTreePv2<bool> q(KVTreePv2<bool>, int level, int nparts);

/** Splits a level in 3 parts,
 * representing the first border,
 * the bulk and the last border. */
KVTreePv2<bool> bb(KVTreePv2<bool>, int level, int halosize);

/** Level collapsing function.
 * Collapses the levels in the range [levelstart,levelend),
 * which are replaced by a single level placed at "levelstart".
 * The keys in the new level are the concatenation
 * of the indices in the collapsed levels. */
KVTreePv2<bool> flatten(KVTreePv2<bool> t, int levelstart, int levelend);

/** Assumes that the relevant dimensions (levels)
 * have already been collapsed with the 'flatten' function,
 * just splits the tree at that level in 2 subtrees,
 * based on the sum of the collapsed indices.
 * It is naive because it is not aware
 * of the parity of the origin of the sublattice
 * corresponding to the subtree.
 * This means that the partitioning between even and odd sites
 * will be correct only up to an exchange between even and odd
 * (that will be different for each subtree). */
KVTreePv2<bool> eo_naive(const KVTreePv2<bool> t, int level);

/** Where all these functions transform an input tree into an output tree,
 * this function transforms an index in the opposite direction
 * (that's why it's called 'pullback').
 * */
template <class Value>
vector<int> index_pullback(const KVTreePv2<Value> &tree, int level_no_id_max,
                           const vector<int> &in) {
  if (level_no_id_max == 0 or in.size() == 0) {
    // we are sure that beyond this point
    // no changes will be made.
    return in;
  } else {
    int idx = in[0];
    if (idx >= tree->children.size()) {
      throw KeyNotFoundError("index over bound");
    }
    auto key = tree->children[idx].first;
    auto subtree = tree->children[idx].second;
    auto out_tail = index_pullback(subtree, level_no_id_max - 1, tail(in));
    vector<int> out;
    std::copy(key.begin(), key.end(), //
              std::back_inserter(out));
    std::copy(out_tail.begin(), out_tail.end(), //
              std::back_inserter(out));
    return out;
  }
}
/* This function checks the keys in the tree
 * and returns the index that matches that.
 * There might be more than one match */
template <class Value>
vector<vector<int>> index_pushforward(const KVTreePv2<Value> &tree,
                                      int level_no_id_max,
                                      const vector<int> &in) {
  if (level_no_id_max == 0 or in.size() == 0) {
    // we are sure that beyond this point
    // no changes will be made.
    return vector<vector<int>>{in};
  } else {
    int keylen = tree->children[0].first.size();
    vector<int> key;
    key.reserve(keylen);
    std::copy(in.begin(), in.begin() + keylen, //
              std::back_inserter(key));
    vector<int> other_indices;
    other_indices.reserve(in.size() - keylen);
    std::copy(in.begin() + keylen, in.end(), //
              std::back_inserter(other_indices));
    vector<vector<int>> sub_results;
    for (auto c = tree->children.begin(); c != tree->children.end(); ++c) {
      if (c->first == key) {
        auto subtree = c->second;
        auto out_tails =
            index_pushforward(subtree, level_no_id_max - 1, other_indices);
        int idx = (int)(c - tree->children.begin());
        for (auto &out_tail : out_tails) {
          vector<int> out = append(idx, out_tail);
          sub_results.push_back(out);
        }
      }
    }
    return sub_results;
  }
}

/** Reshuffles the keys in a level.*/
template <class Value>
KVTreePv2<Value> remap_level(const KVTreePv2<Value> t, int level,
                             vector<int> index_map) {
  decltype(KVTree<Value>::children) children;
  children.reserve(t->children.size());
  if (level == 0) {
    for (int key : index_map) {
      auto c = t->children[key];
      children.push_back({{key}, c.second});
    }

  } else {
    std::transform(t->children.begin(), t->children.end(), //
                   std::back_inserter(children),           //
                   [level, &index_map](const auto &c) {
                     return mp(c.first,
                               remap_level(c.second, level - 1, index_map));
                   });
  }
  return mtkv(t->n, children);
}
} // namespace internals

} // namespace slow
} // namespace hypercubes

#endif // TREE_TRANSFORM_H_
