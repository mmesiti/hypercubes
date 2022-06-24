#ifndef TREE_TRANSFORM_H_
#define TREE_TRANSFORM_H_
#include "exceptions/exceptions.hpp"
#include "geometry/geometry.hpp"
#include "trees/kvtree_data_structure.hpp"
#include "trees/kvtree_v2.hpp"
#include "utils/print_utils.hpp"
#include "utils/utils.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace hypercubes {
namespace slow {
namespace internals {
/** Why have a single class with all the functions inside
 *  instead of many classes, one per function?
 *  The main reason is that some of these functions
 *  call each other (qh and hbb do call renumber_children)
 */
template <class Node> class TreeFactory {
  // TODO: make copy constructor
  //       and copy assignment operator private?
private:
  Node make_leaf();
  Node make_node();
  KVTreePv2<Node> tree_product2(const KVTreePv2<Node> &t1,
                                const KVTreePv2<Node> &t2) {
    if (t1->n == make_leaf()) {
      return t2;
    } else {
      decltype(KVTree<Node>::children) children;
      children.reserve(t1->children.size());
      std::transform(t1->children.begin(), //
                     t1->children.end(),   //
                     std::back_inserter(children), [t2, this](auto c) {
                       return mp(c.first, //
                                 tree_product2(c.second, t2));
                     });
      return mtkv(t1->n, children);
    }
  }

  inline vector<int> sub_level_ordering(const vector<int> &level_ordering) {
    vector<int> res;
    int lvl_removed = level_ordering[0];
    for (int i = 1; i < level_ordering.size(); ++i) {
      int l = level_ordering[i];
      if (l > lvl_removed)
        res.push_back(l - 1);
      else
        res.push_back(l);
    }
    return res;
  }

  void partition_children_into_subtrees(
      decltype(KVTree<Node>::children) &children, //
      const vector<int> &starts, // starts do not have to coincide
      const vector<int> &ends,   // with ends.
      const decltype(KVTree<Node>::children) &t_children,
      BoundaryCondition bc = BoundaryCondition::OPEN) {

    children.reserve(ends.size());
    for (int child = 0; child < ends.size(); child++) {
      int i_grandchildren_start = starts[child];
      int i_grandchildren_end = ends[child];
      decltype(KVTree<Node>::children) grandchildren;
      // TODO: assert size is > 0.
      int grandchildren_size = i_grandchildren_end - i_grandchildren_start;
      grandchildren.reserve(grandchildren_size);

      for (int i = i_grandchildren_start; i < i_grandchildren_end; ++i) {
        int i_sanitised = i;

        // NOTE: Tried to abstract these into methods,
        //       code is longer and less readable.
        if (bc == BoundaryCondition::PERIODIC) {
          if (i_sanitised < 0)
            i_sanitised += t_children.size();
          else if (i_sanitised >= t_children.size())
            i_sanitised -= t_children.size();

          grandchildren.push_back(
              {{i_sanitised}, //
               renumber_children(t_children[i_sanitised].second)});

        } else if (bc == BoundaryCondition::OPEN) {
          if (0 <= i_sanitised and i_sanitised < t_children.size())
            grandchildren.push_back(
                {{i_sanitised}, //
                 renumber_children(t_children[i_sanitised].second)});
          else // add empty node instead
            grandchildren.push_back({{no_key}, mtkv(make_node(), {})});
        }
      }

      if (grandchildren.size() > 0)
        children.push_back(
            {{}, // The new level has no correspondence in the old tree
             mtkv(false, grandchildren)});
    }
  }

  // TODO: consider memoisation
  /** Renumbers subtrees, and applies a function to them.
   *  Helper functions to deal with recursion */
  template <class F>
  KVTreePv2<Node> renumber_children(const KVTreePv2<Node> t, F f) {
    decltype(KVTree<Node>::children) children;
    auto tchildren = t->children;
    children.reserve(tchildren.size());
    for (auto i = 0; i != tchildren.size(); ++i) {
      children.push_back({{i}, f(tchildren[i].second)});
    }
    return mtkv(t->n, children);
  }

  // Caches for memoisation

public:
  // These is public only to expose it conveniently during the tests,
  // so that we have a way to check
  // which functions are called by the higher-level code
  // and with which arguments.
  struct Cache {
    std::map<KVTreePv2<Node>, KVTreePv2<Node>> renumber;

    std::map<std::tuple<KVTreePv2<Node>,    // t
                        int,                // level
                        int,                // nparts
                        int,                // halo
                        int,                // existing_halo
                        BoundaryCondition>, // bc
             KVTreePv2<Node>>
        qh;

    std::map<std::tuple<KVTreePv2<Node>, // t
                        int,             // level
                        int>,            // halosize
             KVTreePv2<Node>>
        bb;

    std::map<std::tuple<KVTreePv2<Node>, // t
                        int,             // level
                        int>,            // halosize
             KVTreePv2<Node>>
        hbb;

    std::map<std::tuple<KVTreePv2<Node>, // t
                        int,             // levelstart
                        int>,            // levelend
             KVTreePv2<Node>>
        flatten;

    std::map<std::tuple<KVTreePv2<Node>, // t
                        int,             // levelstart
                        int>,            // padding
             KVTreePv2<Node>>
        collect_leaves;

    std::map<std::tuple<KVTreePv2<Node>, // t
                        int>,            // level
             KVTreePv2<Node>>
        eo_naive;

    std::map<std::tuple<KVTreePv2<Node>, // t
                        int,             // level
                        vector<int>>,    // index_map
             KVTreePv2<Node>>
        remap_level;

    std::map<std::tuple<KVTreePv2<Node>, // t
                        vector<int>>,    // new_level_ordering
             KVTreePv2<Node>>
        swap_levels;

    // THIS MIGHT NOT BE USEFUL AT ALL
    std::map<std::tuple<KVTreePv2<Node>, // t
                        int>,            // level
             KVTreePv2<Node>>
        bring_level_on_top;

    std::map<std::tuple<KVTreePv2<Node>,  // tree
                        int,              // level to collapse
                        const vector<int> // child key to replace
                        >,
             KVTreePv2<Node>>
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

  KVTreePv2<Node> bring_level_on_top(const KVTreePv2<Node> tree,
                                     int next_level) {

    std::set<vector<int>> keys;
    _collect_keys_at_level(keys, tree, next_level);
    Node root;
    { // check on node types - all nodes should be equal.
      std::set<Node> nodes;
      _collect_nodes_at_level(nodes, tree, next_level);
      if (nodes.size() != 1)
        _throw_different_nodes_error(nodes, std::string("level:") +
                                                std::to_string(next_level));
      else
        root = *nodes.begin();
    }
    decltype(KVTree<Node>::children) children;
    for (auto key : keys) {
      children.push_back({key, collapse_level(tree, next_level, key)});
    }

    return mtkv(root, children);
  }

  // NOTE: Key must be unique between siblings.
  const KVTreePv2<Node>                       //
  collapse_level(const KVTreePv2<Node> &tree, //
                 int level_to_collapse,       //
                 const vector<int> &child_key_to_replace) {
    callcounter.total.collapse_level++;
    if (cache.collapse_level.find(
            {tree, level_to_collapse, child_key_to_replace}) ==
        cache.collapse_level.end()) {
      KVTreePv2<Node> res = 0;
      if (level_to_collapse == 0) {
        for (auto c : tree->children) {
          if (c.first == child_key_to_replace)
            res = c.second;
        }
      } else {
        decltype(KVTree<Node>::children) children;
        children.reserve(tree->children.size());
        for (auto ktree : tree->children) {
          const vector<int> k = ktree.first;
          KVTreePv2<Node> v = collapse_level(
              ktree.second, level_to_collapse - 1, child_key_to_replace);
          if (v)
            children.push_back({k, v});
        }
        res = mtkv(tree->n, children);
      }
      cache.collapse_level[{tree, level_to_collapse, child_key_to_replace}] =
          res;
    } else
      callcounter.cached.collapse_level++;
    return cache
        .collapse_level[{tree, level_to_collapse, child_key_to_replace}];
  }

  void print_diagnostics() {
    int start_colsize = 20;
    int colsize = 15;
    int len = start_colsize + 4 * colsize;
    std::cout << std::string(len, '+') << std::endl;
    // std::cout.width(4 * colsize);
    std::cout << "CACHE COUNTS" << std::endl;
    std::cout << std::string(len, '+') << std::endl;
    std::cout << std::setw(start_colsize) << "TYPE"    //
              << std::setw(colsize) << "Cache Size"    //
              << std::setw(colsize) << "Calls: cached" //
              << std::setw(colsize) << "Calls: total"  //
              << std::setw(colsize) << "Cached/Total" << std::endl;

#define PRINTLINE(FUNC_TYPE)                                      /**/         \
  std::cout << std::setw(start_colsize) << #FUNC_TYPE             /**/         \
            << std::setw(colsize) << cache.FUNC_TYPE.size()       /**/         \
            << std::setw(colsize) << callcounter.cached.FUNC_TYPE /**/         \
            << std::setw(colsize) << callcounter.total.FUNC_TYPE  /**/         \
            << std::setw(colsize)                                              \
            << (callcounter.total.FUNC_TYPE == 0                               \
                    ? 0                                                        \
                    : (float)callcounter.cached.FUNC_TYPE /                    \
                          callcounter.total.FUNC_TYPE)                         \
            << std::endl;

    PRINTLINE(renumber)
    PRINTLINE(qh)
    PRINTLINE(bb)
    PRINTLINE(hbb)
    PRINTLINE(flatten)
    PRINTLINE(collect_leaves)
    PRINTLINE(eo_naive)
    PRINTLINE(remap_level)
    PRINTLINE(swap_levels)
    PRINTLINE(bring_level_on_top)
    PRINTLINE(collapse_level)

#undef PRINTLINE
    std::cout << std::string(4 * colsize, '+') << std::endl;
  }
  KVTreePv2<Node> generate_flat_level(int size) {
    auto leaf = mtkv(make_leaf(), {});
    decltype(KVTree<Node>::children) children;
    children.reserve(size);
    for (int i = 0; i < size; ++i) {
      children.push_back({{i}, leaf});
    }
    return mtkv(make_node(), children);
  }

  /** Renumbers subtrees recursively.
   *  Helper functions to deal with recursion,
   *  but also for those cases where the keys must be renumbered
   *  (e.g., before swap_levels after qh or bb)
   *  Recursive function. */
  KVTreePv2<Node> renumber_children(const KVTreePv2<Node> t) {
    callcounter.total.renumber++;
    if (cache.renumber.find(t) == cache.renumber.end()) {
      decltype(KVTree<Node>::children) children;
      auto tchildren = t->children;
      children.reserve(tchildren.size());
      for (auto i = 0; i != tchildren.size(); ++i) {
        children.push_back({{i}, renumber_children(tchildren[i].second)});
      }
      auto res = mtkv(t->n, children);
      cache.renumber[t] = res;
      cache.renumber[res] = res; // It is idempotent
    } else
      callcounter.cached.renumber++;
    return cache.renumber[t];
  }

  KVTreePv2<Node> tree_product(const vector<KVTreePv2<Node>> &trees) {
    if (trees.size() == 1) {
      return trees[0];
    } else {
      return tree_product2(trees[0], tree_product(tail(trees)));
    }
  };

  KVTreePv2<Node> tree_sum(const vector<KVTreePv2<Node>> &trees) {
    decltype(KVTree<Node>::children) children;
    children.reserve(trees.size());
    for (int i = 0; i < trees.size(); ++i)
      children.push_back({{}, trees[i]});
    return mtkv(false, children);
  }

  /** Generates a very simple tree
   * representing a N-dimensional array
   * from a list of dimensions.
   * It is also guaranteed yield an identity transformation
   * when used with index_pullback.
   * */
  KVTreePv2<Node> generate_nd_tree(std::vector<int> dimensions) {
    vector<KVTreePv2<Node>> levels;
    levels.reserve(dimensions.size());
    std::transform(dimensions.begin(),         //
                   dimensions.end(),           //
                   std::back_inserter(levels), //
                   [this](int s) { return generate_flat_level(s); });
    return tree_product(levels);
  }

  /** Level splitting functions.
   * The new level uses {} for all the keys
   * as there is no correspondent level in the input tree.
   * NOTE: the subtrees at depth level+1 are untouched
   *       and concide with the relative subtrees in the input tree.
   *       This means that they should be ignored by index_pullback,
   *       or that the children should be renumbered so that index_pullback does
   *       the right thing.   */

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
  KVTreePv2<Node> qh(KVTreePv2<Node> t, //
                     int level,         //
                     int nparts,        //
                     int halo,          //
                     int existing_halo, //
                     BoundaryCondition bc) {
    // TODO:
    // There are some complex conditions regarding
    // halo, existing halo, the levels and the boundary conditions.
    // e.g., the halo should be smaller or equal to the existing halo,
    // except if the level to split is the whole lattice.
    // Also, it may make little sense to have PERIODIC bcs
    // on sublevels.
    // It might make more sense to have a function/transformer/...
    // for the full lattice case (where it is possible to specify the boundary
    // condition) and another where "OPEN" is always the case, for levels down
    // the hierarchy.
    // This can be managed at the TransformRequestMaker level, though.
    callcounter.total.qh++;
    if (cache.qh.find({t, level, nparts, halo, existing_halo, bc}) ==
        cache.qh.end()) {

      KVTreePv2<Node> res;
      decltype(KVTree<Node>::children) children;
      int size = t->children.size();
      if (level == 0) {
        float quotient = (float)(size - 2 * existing_halo) / nparts;
        vector<int> starts, ends;
        starts.reserve(nparts);
        ends.reserve(nparts);
        for (int child = 0; child < nparts; child++) {
          int start = std::round(child * quotient) + existing_halo - halo;
          int end = std::round((child + 1) * quotient) + existing_halo + halo;

          starts.push_back(start);
          ends.push_back(end);
        }
        partition_children_into_subtrees(children, starts, ends, t->children,
                                         bc);
        res = mtkv(false, children);
      } else {
        res = renumber_children(
            t, [this, level, nparts, halo, existing_halo, bc](auto subtree) {
              return qh(subtree, level - 1, nparts, halo, existing_halo, bc);
            });
      }
      cache.qh[{t, level, nparts, halo, existing_halo, bc}] = res;
    } else
      callcounter.cached.qh++;
    return cache.qh[{t, level, nparts, halo, existing_halo, bc}];
  }

  /** Splits a level in 3 parts,
   * representing the first border,
   * the bulk and the last border.
   * Affects only level and level+1.
   * Uses recursion. */
  KVTreePv2<Node> bb(KVTreePv2<Node> t, int level, int halo) {
    callcounter.total.bb++;
    if (cache.bb.find({t, level, halo}) == cache.bb.end()) {

      KVTreePv2<Node> res;
      decltype(KVTree<Node>::children) children;
      int size = t->children.size();
      if (level == 0) {
        std::vector<int> limits = {0,           //
                                   halo,        //
                                   size - halo, //
                                   size};
        auto starts = limits; // except the last...
        auto ends = tail(limits);
        partition_children_into_subtrees(children, starts, ends, t->children);
        res = mtkv(false, children);
      } else {
        res = renumber_children(t, [this, level, halo](auto subtree) {
          return bb(subtree, level - 1, halo);
        });
      }
      cache.bb[{t, level, halo}] = res;
    } else
      callcounter.cached.bb++;
    return cache.bb[{t, level, halo}];
  }
  /** Splits a level in 5 parts,
   * representing the first halo,
   * the first border,
   * the bulk, the last border
   * and the last halo.
   * Should (must?) be used after qh with the same halosize.
   * Affects only level and level+1.
   * Uses recursion. */
  KVTreePv2<Node> hbb(KVTreePv2<Node> t, int level, int halo) {
    callcounter.total.hbb++;
    if (cache.hbb.find({t, level, halo}) == cache.hbb.end()) {

      KVTreePv2<Node> res;
      if (level == 0) {
        decltype(KVTree<Node>::children) children;
        int size = t->children.size() - 2 * halo;
        if (size <= 2 * halo) {
          throw std::invalid_argument(
              "HBB: the halo does not fit in the domain!");
        }
        std::vector<int> limits = {0,                  //
                                   halo,               //
                                   2 * halo,           //
                                   halo + size - halo, //
                                   halo + size,        //
                                   halo + size + halo};
        auto starts = limits; // except the last...
        auto ends = tail(limits);
        partition_children_into_subtrees(children, starts, ends, t->children);
        res = mtkv(false, children);
      } else {
        res = renumber_children(t, [this, level, halo](auto subtree) {
          return hbb(subtree, level - 1, halo);
        });
      }
      cache.hbb[{t, level, halo}] = res;
    } else
      callcounter.cached.hbb++;
    return cache.hbb[{t, level, halo}];
  }

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

  KVTreePv2<Node> flatten(KVTreePv2<Node> t, //
                          int levelstart,    //
                          int levelend) {
    callcounter.total.flatten++;
    if (cache.flatten.find({t, levelstart, levelend}) == cache.flatten.end()) {
      KVTreePv2<Node> res;
      decltype(KVTree<Node>::children) children;
      if (levelstart == 0) {
        if (levelend > 1) {
          for (auto i = 0; i != t->children.size(); ++i) {
            auto tchild = flatten(t->children[i].second, 0, levelend - 1);
            if (tchild)
              for (auto grandchild : tchild->children) {
                auto keys = append(i, grandchild.first);
                if (grandchild.second)
                  children.push_back({keys, grandchild.second});
              }
          }
        } else {
          for (auto i = 0; i != t->children.size(); ++i) {
            auto tchild = t->children[i].second;
            children.push_back({{i}, tchild});
          }
        }
        res = mtkv(false, children);
      } else {
        res = renumber_children(t, //
                                [this, levelstart, levelend](auto subtree) {
                                  return flatten(subtree, levelstart - 1,
                                                 levelend - 1);
                                });
      }
      cache.flatten[{t, levelstart, levelend}] = res;
    } else
      callcounter.cached.flatten++;
    return cache.flatten[{t, levelstart, levelend}];
  }

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
  KVTreePv2<Node> collect_leaves(KVTreePv2<Node> t, //
                                 int levelstart,    //
                                 int pad_to) {

    callcounter.total.collect_leaves++;
    if (cache.collect_leaves.find({t, levelstart, pad_to}) ==
        cache.collect_leaves.end()) {
      KVTreePv2<Node> res;
      decltype(KVTree<Node>::children) children;
      if (levelstart <= 0) {
        if (t->n != make_leaf()) {
          for (auto i = 0; i != t->children.size(); ++i) {
            auto tchild = collect_leaves(t->children[i].second, //
                                         levelstart - 1,        //
                                         pad_to);
            for (auto grandchild : tchild->children) {
              auto keys = append(i, grandchild.first);
              children.push_back({keys, grandchild.second});
            }
          }
        } else {
          children.push_back({{}, t});
        }
        if (levelstart == 0) {
          auto leaf = mtkv(make_leaf(), {});
          // TODO: This may cause problems
          //                    V
          int keylen = children[0].first.size();
          std::vector<int> key(keylen, no_key);
          children.reserve(pad_to);
          while (children.size() < pad_to)
            children.push_back({key, leaf});
        }
        res = mtkv(false, children);
      } else {
        res = renumber_children(t,           //
                                [this,       //
                                 levelstart, //
                                 pad_to](auto subtree) {
                                  return collect_leaves(subtree,        //
                                                        levelstart - 1, //
                                                        pad_to);
                                });
      }
      cache.collect_leaves[{t, levelstart, pad_to}] = res;
    } else
      callcounter.cached.collect_leaves++;
    return cache.collect_leaves[{t, levelstart, pad_to}];
  }

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
  KVTreePv2<Node> eo_naive(const KVTreePv2<Node> t, int level) {

    callcounter.total.eo_naive++;
    if (cache.eo_naive.find({t, level}) == cache.eo_naive.end()) {

      KVTreePv2<Node> res;
      decltype(KVTree<Node>::children) children;
      if (level == 0) {
        children.reserve(2);
        decltype(children) E, O;
        decltype(children) EO[2] = {E, O};
        for (auto i = 0; i < t->children.size(); ++i) {
          auto keys = t->children[i].first;
          EO[std::accumulate(keys.begin(), keys.end(), 0) % 2].push_back(
              {{i}, t->children[i].second});
        }
        for (auto eo : EO) {
          if (eo.size() > 0)
            children.push_back({{}, mtkv(false, eo)});
        }
        res = mtkv(false, children);
      } else {
        res = renumber_children(t, //
                                [this, level](auto subtree) {
                                  return eo_naive(subtree, level - 1);
                                });
      }
      cache.eo_naive[{t, level}] = res;
    } else
      callcounter.cached.eo_naive++;
    return cache.eo_naive[{t, level}];
  }

  /** Remaps keys in a level, allowing duplications and deletions. */
  KVTreePv2<Node> remap_level(const KVTreePv2<Node> t, int level,
                              vector<int> index_map) {

    callcounter.total.remap_level++;
    if (cache.remap_level.find({t, level, index_map}) ==
        cache.remap_level.end()) {
      KVTreePv2<Node> res;
      decltype(KVTree<Node>::children) children;
      children.reserve(t->children.size());
      if (level == 0) {
        for (int key : index_map) {
          auto c = t->children[key];
          children.push_back({{key}, renumber_children(c.second)});
        }

        res = mtkv(t->n, children);
      } else {
        res = renumber_children(t, [this, level, index_map](auto subtree) {
          return remap_level(subtree, level - 1, index_map);
        });
      }
      cache.remap_level[{t, level, index_map}] = res;
    } else
      callcounter.cached.remap_level++;
    return cache.remap_level[{t, level, index_map}];
  }

  const KVTreePv2<Node> swap_levels(const KVTreePv2<Node> &t,
                                    const vector<int> &new_level_ordering) {

    callcounter.total.swap_levels++;
    if (cache.swap_levels.find({t, new_level_ordering}) ==
        cache.swap_levels.end()) {

      KVTreePv2<Node> res;
      if (new_level_ordering.size() == 0)
        res = t;
      else {
        int next_level = new_level_ordering[0];
        auto new_t = bring_level_on_top(t, next_level);
        if (new_t == 0)
          return 0;

        auto sub_new_level_ordering = sub_level_ordering(new_level_ordering);

        decltype(t->children) new_children;
        new_children.reserve(t->children.size());
        for (const auto &c : new_t->children) {
          auto new_child = swap_levels(c.second, sub_new_level_ordering);
          if (new_child != 0)
            new_children.push_back({c.first, new_child});
        }

        res = mtkv(new_t->n, new_children);
      }
      cache.swap_levels[{t, new_level_ordering}] = res;
    } else {
      callcounter.cached.swap_levels++;
    }

    return cache.swap_levels[{t, new_level_ordering}];
  }
}; // namespace internals

// Defining this inside the class body
// causes a linking error
// at low optimisation.
// Solution from
// https://stackoverflow.com/questions/3229883/static-member-initialization-in-a-class-template
template <typename T> const int TreeFactory<T>::no_key = -1;

// template <> class TreeFactory<bool>;
// template <> bool TreeFactory<bool>::make_leaf();
// template <> bool TreeFactory<bool>::make_node();

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
                TreeFactory<Value>::no_key) == out_temp.end()) {
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
  if (idx == TreeFactory<Value>::no_key or idx >= tree->children.size()) {
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
