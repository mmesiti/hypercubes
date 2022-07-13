#ifndef TREE_TRANSFORM_H_
#define TREE_TRANSFORM_H_
#include "exceptions/exceptions.hpp"
#include "geometry/geometry.hpp"
#include "selectors/bool_maybe.hpp"
#include "trees/kvtree_data_structure.hpp"
#include "trees/kvtree_v2.hpp"
#include "utils/print_utils.hpp"
#include "utils/utils.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace hypercubes {
namespace slow {
namespace internals {

enum NodeType {
  NODE, // The usual node
  LEAF, // The leaf
};

/** Why have a single class with all the functions inside
 *  instead of many classes, one per function?
 *  The main reason is that some of these functions
 *  call each other (qh and hbb do call renumber_children)
 */
class TreeFactory {
public:
  using Predicate = std::function<BoolM(const std::vector<int> &)>;
  // TODO: make copy constructor
  //       and copy assignment operator private?
private:
  NodeType make_leaf();
  NodeType make_node();
  KVTreePv2<NodeType> tree_product2(const KVTreePv2<NodeType> &t1,
                                    const KVTreePv2<NodeType> &t2);
  vector<int> sub_level_ordering(const vector<int> &level_ordering);

  void partition_children_into_subtrees(
      decltype(KVTree<NodeType>::children) &children, //
      const vector<int> &starts, // starts do not have to coincide
      const vector<int> &ends,   // with ends.
      const decltype(KVTree<NodeType>::children) &t_children,
      BoundaryCondition bc = BoundaryCondition::OPEN);

  // TODO: consider memoisation
  /** Renumbers subtrees, and applies a function to them.
   *  Helper functions to deal with recursion */
  template <class F>
  KVTreePv2<NodeType> renumber_children(const KVTreePv2<NodeType> t, F f) {
    decltype(KVTree<NodeType>::children) children;
    auto tchildren = t->children;
    children.reserve(tchildren.size());
    for (auto i = 0; i != tchildren.size(); ++i) {
      children.push_back({{i}, f(tchildren[i].second)});
    }
    return mtkv(t->n, children);
  }

  // Caches for memoisation
  KVTreePv2<NodeType> _select_subtree(const KVTreePv2<NodeType> t, //
                                      std::vector<int> idx_above,  //
                                      Predicate &p);

public:
  // These is public only to expose it conveniently during the tests,
  // so that we have a way to check
  // which functions are called by the higher-level code
  // and with which arguments.
  struct Cache {
    std::map<KVTreePv2<NodeType>, KVTreePv2<NodeType>> renumber;

    std::map<std::tuple<KVTreePv2<NodeType>, // t
                        int,                 // level
                        int,                 // nparts
                        int,                 // halo
                        int,                 // existing_halo
                        BoundaryCondition>,  // bc
             KVTreePv2<NodeType>>
        qh;

    std::map<std::tuple<KVTreePv2<NodeType>, // t
                        int,                 // level
                        int>,                // halosize
             KVTreePv2<NodeType>>
        bb;

    std::map<std::tuple<KVTreePv2<NodeType>, // t
                        int,                 // level
                        int>,                // halosize
             KVTreePv2<NodeType>>
        hbb;

    std::map<std::tuple<KVTreePv2<NodeType>, // t
                        int,                 // levelstart
                        int>,                // levelend
             KVTreePv2<NodeType>>
        flatten;

    std::map<std::tuple<KVTreePv2<NodeType>, // t
                        int,                 // levelstart
                        int>,                // padding
             KVTreePv2<NodeType>>
        collect_leaves;

    std::map<std::tuple<KVTreePv2<NodeType>, // t
                        int>,                // level
             KVTreePv2<NodeType>>
        eo_naive;

    std::map<std::tuple<KVTreePv2<NodeType>, // t
                        int,                 // level
                        vector<int>>,        // index_map
             KVTreePv2<NodeType>>
        remap_level;

    std::map<std::tuple<KVTreePv2<NodeType>, // t
                        vector<int>>,        // new_level_ordering
             KVTreePv2<NodeType>>
        swap_levels;

    // THIS MIGHT NOT BE USEFUL AT ALL
    std::map<std::tuple<KVTreePv2<NodeType>, // t
                        int>,                // level
             KVTreePv2<NodeType>>
        bring_level_on_top;

    std::map<std::tuple<KVTreePv2<NodeType>, // tree
                        int,                 // level to collapse
                        const vector<int>    // child key to replace
                        >,
             KVTreePv2<NodeType>>
        collapse_level;
  } cache;
  struct CallCounter {
    struct Counts {
      int renumber = 0;
      int qh = 0;
      int bb = 0;
      int hbb = 0;
      int flatten = 0;
      int collect_leaves = 0;
      int eo_naive = 0;
      int remap_level = 0;
      int swap_levels = 0;
      int bring_level_on_top = 0;
      int collapse_level = 0;
    } total, cached;

  } callcounter;

  KVTreePv2<NodeType> bring_level_on_top(const KVTreePv2<NodeType> tree,
                                         int next_level);

  // NOTE: Key must be unique between siblings.
  const KVTreePv2<NodeType>                       //
  collapse_level(const KVTreePv2<NodeType> &tree, //
                 int level_to_collapse,           //
                 const vector<int> &child_key_to_replace);

  void print_diagnostics();
  KVTreePv2<NodeType> generate_flat_level(int size);

  /** Renumbers subtrees recursively.
   *  Helper functions to deal with recursion,
   *  but also for those cases where the keys must be renumbered
   *  (e.g., before swap_levels after qh or bb)
   *  Recursive function. */
  KVTreePv2<NodeType> renumber_children(const KVTreePv2<NodeType> t);

  KVTreePv2<NodeType> tree_product(const vector<KVTreePv2<NodeType>> &trees);

  KVTreePv2<NodeType> tree_sum(const vector<KVTreePv2<NodeType>> &trees);

  /** Generates a very simple tree
   * representing a N-dimensional array
   * from a list of dimensions.
   * It is also guaranteed yield an identity transformation
   * when used with index_pullback.
   * */
  KVTreePv2<NodeType> generate_nd_tree(std::vector<int> dimensions);

  /** Level splitting functions.
   * The new level uses {} for all the keys
   * as there is no correspondent level in the input tree.
   * NOTE: the subtrees at depth level+1 are untouched
   *       and concide with the relative subtrees in the input tree.
   *       This means that they should be ignored by index_pullback,
   *       or that the children should be renumbered so that index_pullback
   * does the right thing.   */

  /** Splits a level of a tree in n-parts,
   * making sure the partitions are as equal as possible,
   * and adding a halo of specified size on each partition.
   * Assumes that the level has already a halo of size "existing_halo",
   * so that the halo in the new level
   * will have a superposition with the existing halo.
   * Affects only level and level+1.
   *
   * Uses recursion.
   *
   * NOTE: This function might not create all the subtrees.
   *       This happens ONLY when using open boundary conditions,
   *       at the edge of the range, for halo > 0.
   * */

  // TODO: existing halo must be zero when periodic,
  //       so it makes sense to have two different versions
  //       of this function - one with open boundary conditions
  //       accepting a "existing halo" parameter,
  //       and another with periodic boundary conditions
  //       without that parameter
  //       (since periodic boundary conditions
  //       make sense only for the full lattice,
  //       so there is no existing halo)
  KVTreePv2<NodeType> qh(KVTreePv2<NodeType> t, //
                         int level,             //
                         int nparts,            //
                         int halo,              //
                         int existing_halo,     //
                         BoundaryCondition bc);

  /** Splits a level in 3 parts,
   * representing the first border,
   * the bulk and the last border.
   * Affects only level and level+1.
   * Uses recursion. */
  KVTreePv2<NodeType> bb(KVTreePv2<NodeType> t, int level, int halo);
  /** Splits a level in 5 parts,
   * representing the first halo,
   * the first border,
   * the bulk, the last border
   * and the last halo.
   * Should (must?) be used after qh with the same halosize.
   * Affects only level and level+1.
   * Uses recursion. */
  KVTreePv2<NodeType> hbb(KVTreePv2<NodeType> t, int level, int halo);

  /** Level collapsing function.
   * Collapses the levels in the range [levelstart,levelend),
   * which are replaced by a single level placed at "levelstart",
   * using a depth-first path.
   * The keys in the new level are the concatenation
   * of the indices in the collapsed levels.
   * Affects only the levels in said range.
   * Uses recursion.
   * NOTE: this function is the inverse of q and bb.
   *       it should also be the inverse of hbb.
   *       qh, though produces copies when halo != 0.*/

  KVTreePv2<NodeType> flatten(KVTreePv2<NodeType> t, //
                              int levelstart,        //
                              int levelend);

  /** If a leaf was created as a result of padding,
   * all the components of its key will be this value. */
  static const int no_key;

  /** Collects leaves of all subtree
   * having a root at level "levelstart-1".
   * The level "levelstart" is then replaced
   * with a list of leaves
   * which is at least "pad_to" long.
   *
   * The keys in the new level are the concatenation
   * of the indices in the collapsed levels.
   * Affects only the levels in said range.
   * Uses recursion.
   * NOTE: For trees produced by this function
   *       the "index_pullback()" function
   *       might raise exceptions,
   *       as not all the leaves in the output tree
   *       have a corresponding leaf in the input tree.
   */
  KVTreePv2<NodeType> collect_leaves(KVTreePv2<NodeType> t, //
                                     int levelstart,        //
                                     int pad_to);

  /** Assumes that the relevant dimensions (levels)
   * have already been collapsed with the 'flatten' function,
   * just splits the tree at that level in 2 subtrees,
   * based on the sum of the collapsed indices.
   * It is naive because it is not aware
   * of the parity of the origin of the sublattice
   * corresponding to the subtree.
   * This means that the partitioning between even and odd sites
   * will be correct only up to an exchange between even and odd
   * (that will be different for each subtree).
   * Therefore, an additional step might be necessary
   * to fix the parity.
   * Uses recursion. */
  KVTreePv2<NodeType> eo_naive(const KVTreePv2<NodeType> t, int level);

  /** Remaps keys in a level, allowing duplications and deletions. */
  KVTreePv2<NodeType> remap_level(const KVTreePv2<NodeType> t, int level,
                                  vector<int> index_map);

  KVTreePv2<NodeType> swap_levels(const KVTreePv2<NodeType> t,
                                  const vector<int> &new_level_ordering);

  KVTreePv2<NodeType> select_subtree(const KVTreePv2<NodeType> t, Predicate &);
};

/** Where all these functions transform an input tree into an output tree,
 * this function transforms an index in the opposite direction
 * (that's why it's called 'pullback').
 * */

template <class Value>
vector<int> index_pullback(const KVTreePv2<Value> &tree,
                           const vector<int> &in) {
  vector<int> out;
  _index_pullback(tree, in, out);
  return out;
}

/* This function covers the case
 * where padding creates some new leaves
 * that have no correspondence in the old tree.
 * In that case we return an empty vector.  */
template <class Value>
vector<vector<int>> index_pullback_safe(const KVTreePv2<Value> &tree,
                                        const vector<int> &in) {
  vector<vector<int>> out;
  auto out_temp = index_pullback(tree, in);
  if (std::find(out_temp.begin(), //
                out_temp.end(),   //
                TreeFactory::no_key) == out_temp.end()) {
    out.push_back(out_temp);
  }
  return out;
}

static inline void throw_index_pullback_error(int key, int size) {
  std::stringstream ss;
  ss << "Index over bound: (unsigned)" << key << "<" << size;
  throw KeyNotFoundError(ss.str());
}

template <class Value>
void _index_pullback(const KVTreePv2<Value> &tree, //
                     const vector<int> &in,        //
                     vector<int> &out) {
  if (in.size() == 0)
    return;

  int idx = in[0];
  if (idx == TreeFactory::no_key or idx >= tree->children.size()) {
    throw_index_pullback_error(idx, tree->children.size());
  }

  {
    vector<int> key = tree->children[idx].first;
    std::copy(key.begin(), key.end(), //
              std::back_inserter(out));
  }
  auto subtree = tree->children[idx].second;

  _index_pullback(subtree, tail(in), out); // TCO, hopefully
}
/* This function checks the keys in the tree
 * and returns the index that matches that.
 * There might be more than one match */
template <class Value>
vector<vector<int>> index_pushforward(const KVTreePv2<Value> &tree,
                                      const vector<int> &in) {

  vector<vector<int>> sub_results;
  _index_pushforward(tree, in, {}, sub_results);
  return sub_results;
}

template <class Value>
void _index_pushforward(const KVTreePv2<Value> &tree, //
                        const vector<int> &in,        //
                        vector<int> &&result,         //
                        vector<vector<int>> &all_results) {
  if (in.size() == 0)
    all_results.push_back(result);
  else {
    int keylen = tree->children[0].first.size();
    vector<int> key;
    vector<int> other_indices;
    { // splitting 'in' in key and other_indices
      key.reserve(keylen);
      std::copy(in.begin(), in.begin() + keylen, //
                std::back_inserter(key));
      other_indices.reserve(in.size() - keylen);
      std::copy(in.begin() + keylen, in.end(), //
                std::back_inserter(other_indices));
    }

    const int matches = std::count_if(tree->children.begin(), //
                                      tree->children.end(),   //
                                      [key](auto c) { return c.first == key; });

    int match_count = 0;
    for (auto c = tree->children.begin(); c != tree->children.end(); ++c) {
      if (c->first == key) {
        match_count++;
        int idx = (int)(c - tree->children.begin());
        if (match_count < matches) { // need to create new result
          vector<int> result_copy;
          result_copy = vector<int>(result);
          result_copy.push_back(idx);
          _index_pushforward(c->second,              //
                             other_indices,          //
                             std::move(result_copy), //
                             all_results);
        } else { // last match: can reuse argument
          result.push_back(idx);
          _index_pushforward(c->second,         //
                             other_indices,     //
                             std::move(result), //
                             all_results);
        }
      }
    }
  }
}

} // namespace internals

} // namespace slow
} // namespace hypercubes

#endif // TREE_TRANSFORM_H_
