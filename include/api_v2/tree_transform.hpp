#ifndef TREE_TRANSFORM_H_
#define TREE_TRANSFORM_H_
#include "exceptions/exceptions.hpp"
#include "trees/kvtree_data_structure.hpp"
#include "trees/kvtree_v2.hpp"
#include "utils/print_utils.hpp"
#include "utils/utils.hpp"
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <vector>

namespace hypercubes {
namespace slow {
namespace internals {

template <class Node> class TreeFactory {
private:
  Node make_leaf();
  Node make_node();
  KVTreePv2<Node> tree_product2(const KVTreePv2<Node> &t1,
                                const KVTreePv2<Node> &t2) {
    if (t1->n == true) {
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
      const vector<int> &starts,                  //
      const vector<int> &ends,                    //
      const decltype(KVTree<Node>::children) &t_children) {

    children.reserve(ends.size());
    for (int child = 0; child < ends.size(); child++) {
      int i_grandchildren_start = starts[child];
      int i_grandchildren_end = ends[child];
      decltype(KVTree<Node>::children) grandchildren;
      grandchildren.reserve(i_grandchildren_end - i_grandchildren_start);

      for (auto i = i_grandchildren_start; i < i_grandchildren_end; ++i) {

        grandchildren.push_back({{i}, //
                                 renumber_children(t_children[i].second)});
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
  struct Cache {
    std::map<KVTreePv2<Node>, KVTreePv2<Node>> renumber;

    std::map<std::tuple<KVTreePv2<Node>, // t
                        int,             // level
                        int>,            // nparts
             KVTreePv2<Node>>
        q;

    std::map<std::tuple<KVTreePv2<Node>, // t
                        int,             // level
                        int>,            // halosize
             KVTreePv2<Node>>
        bb;

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
      int q = 0;
      int bb = 0;
      int flatten = 0;
      int collect_leaves = 0;
      int eo_naive = 0;
      int remap_level = 0;
      int swap_levels = 0;
      int bring_level_on_top = 0;
      int collapse_level = 0;
    } total, cached;

  } callcounter;

public:
  // TODO: check that memoisation is useful here. Likely not.
  KVTreePv2<Node> bring_level_on_top(const KVTreePv2<Node> tree,
                                     int next_level) {
    callcounter.total.bring_level_on_top++;                  // REMOVE?
    if (cache.bring_level_on_top.find({tree, next_level}) == // REMOVE?
        cache.bring_level_on_top.end()) {                    // REMOVE?
      std::set<vector<int>> keys;
      _collect_keys_at_level(keys, tree, next_level);
      std::set<Node> nodes;
      _collect_nodes_at_level(nodes, tree, next_level);
      if (nodes.size() == 0) {
        // return 0;
        cache.bring_level_on_top[{tree, next_level}] = 0; // REMOVE?
      } else if (nodes.size() != 1)
        _throw_different_nodes_error(nodes, std::string("level:") +
                                                std::to_string(next_level));
      else {
        decltype(KVTree<Node>::children) children;
        for (auto key : keys) {
          children.push_back({key, collapse_level(tree, next_level, key)});
        }

        // return mtkv(*nodes.begin(), children); // UNCOMMENT
        cache.bring_level_on_top[{tree, next_level}] = // REMOVE?
            mtkv(*nodes.begin(), children);            // REMOVE?
      }
    } else                                               // REMOVE?
      callcounter.cached.bring_level_on_top++;           // REMOVE?
                                                         // REMOVE?
    return cache.bring_level_on_top[{tree, next_level}]; // REMOVE?
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
    PRINTLINE(q)
    PRINTLINE(bb)
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
   *  (e.g., before swap_levels and after q or bb)
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
   *       This means that they should be ignored by index_pullback. */

  /** Splits a level of a tree in n-parts,
   * making sure the partitions are as equal as possible.
   * Affects only level and level+1.
   * Uses recursion. */
  KVTreePv2<Node> q(KVTreePv2<Node> t, int level, int nparts) {
    callcounter.total.q++;
    if (cache.q.find({t, level, nparts}) == cache.q.end()) {

      KVTreePv2<Node> res;
      decltype(KVTree<Node>::children) children;
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
        partition_children_into_subtrees(children, starts, ends, t->children);
        res = mtkv(false, children);
      } else {
        res = renumber_children(t, [this, level, nparts](auto subtree) {
          return q(subtree, level - 1, nparts);
        });
      }
      cache.q[{t, level, nparts}] = res;
    } else
      callcounter.cached.q++;
    return cache.q[{t, level, nparts}];
  }
  /** Splits a level in 3 parts,
   * representing the first border,
   * the bulk and the last border.
   * Affects only level and level+1.
   * Uses recursion. */
  KVTreePv2<Node> bb(KVTreePv2<Node> t, int level, int halosize) {
    callcounter.total.bb++;
    if (cache.bb.find({t, level, halosize}) == cache.bb.end()) {

      KVTreePv2<Node> res;
      decltype(KVTree<Node>::children) children;
      int size = t->children.size();
      if (level == 0) {
        std::vector<int> limits = {0,               //
                                   halosize,        //
                                   size - halosize, //
                                   size};
        auto starts = limits; // except the last...
        auto ends = tail(limits);
        partition_children_into_subtrees(children, starts, ends, t->children);
        res = mtkv(false, children);
      } else {
        res = renumber_children(t, [this, level, halosize](auto subtree) {
          return bb(subtree, level - 1, halosize);
        });
      }
      cache.bb[{t, level, halosize}] = res;
    } else
      callcounter.cached.bb++;
    return cache.bb[{t, level, halosize}];
  }
  /** Level collapsing function.
   * Collapses the levels in the range [levelstart,levelend),
   * which are replaced by a single level placed at "levelstart".
   * The keys in the new level are the concatenation
   * of the indices in the collapsed levels.
   * Affects only the levels in said range.
   * Uses recursion. */
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
          int keylen = children[0].first.size();
          std::vector<int> key(keylen, -1);
          children.reserve(pad_to);
          while (children.size() < pad_to)
            children.push_back({key, leaf});
        }
        res = mtkv(false, children);
      } else {
        res = renumber_children(t, //
                                [this, levelstart, pad_to](auto subtree) {
                                  return collect_leaves(subtree, levelstart - 1,
                                                        pad_to - 1);
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

  /** Where all these functions transform an input tree into an output tree,
   * this function transforms an index in the opposite direction
   * (that's why it's called 'pullback').
   * */
  /** Reshuffles the keys in a level.*/
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

  const KVTreePv2<Node> swap_levels(const KVTreePv2<Node> &tree,
                                    const vector<int> &new_level_ordering) {
    callcounter.total.swap_levels++;
    if (cache.swap_levels.find({tree, new_level_ordering}) ==
        cache.swap_levels.end()) {

      KVTreePv2<Node> res;
      if (new_level_ordering.size() == 0)
        res = tree;
      else {
        int next_level = new_level_ordering[0];
        auto new_tree = bring_level_on_top(tree, next_level);
        if (new_tree == 0)
          return 0;

        auto sub_new_level_ordering = sub_level_ordering(new_level_ordering);

        decltype(tree->children) new_children;
        new_children.reserve(tree->children.size());
        for (const auto &c : new_tree->children) {
          auto new_child = swap_levels(c.second, sub_new_level_ordering);
          if (new_child != 0)
            new_children.push_back({c.first, new_child});
        }

        res = mtkv(new_tree->n, new_children);
      }
      cache.swap_levels[{tree, new_level_ordering}] = res;
    } else {
      callcounter.cached.swap_levels++;
    }

    return cache.swap_levels[{tree, new_level_ordering}];
  }
};

template <> bool TreeFactory<bool>::make_leaf();
template <> bool TreeFactory<bool>::make_node();
template <class Value>
vector<int> index_pullback(const KVTreePv2<Value> &tree,
                           const vector<int> &in) {
  vector<int> out;
  _index_pullback(tree, in, out);
  return out;
}

template <class Value>
void _index_pullback(const KVTreePv2<Value> &tree, //
                     const vector<int> &in,        //
                     vector<int> &out) {
  if (in.size() == 0)
    return;

  int idx = in[0];
  if (idx >= tree->children.size()) {
    throw KeyNotFoundError("index over bound");
  }

  auto subtree = tree->children[idx].second;

  {
    vector<int> key;
    key = tree->children[idx].first;
    std::copy(key.begin(), key.end(), //
              std::back_inserter(out));
  }

  _index_pullback(subtree, tail(in), out); // TCO
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
    int matches = 0;
    for (auto c = tree->children.begin(); c != tree->children.end(); ++c)
      if (c->first == key)
        matches++;

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
