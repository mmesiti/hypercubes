#ifndef TREE_TRANSFORM_H_
#define TREE_TRANSFORM_H_
#include "exceptions/exceptions.hpp"
#include "trees/kvtree_data_structure.hpp"
#include "trees/kvtree_v2.hpp"
#include "utils/utils.hpp"
#include <cassert>
#include <cmath>
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
      decltype(KVTree<bool>::children) &children, //
      const vector<int> &starts,                  //
      const vector<int> &ends,                    //
      const decltype(KVTree<bool>::children) &t_children) {

    children.reserve(ends.size());
    for (int child = 0; child < ends.size(); child++) {
      int i_grandchildren_start = starts[child];
      int i_grandchildren_end = ends[child];
      decltype(KVTree<bool>::children) grandchildren;
      grandchildren.reserve(i_grandchildren_end - i_grandchildren_start);

      for (auto i = i_grandchildren_start; i < i_grandchildren_end; ++i)
        grandchildren.push_back({{i}, //
                                 renumber_children(t_children[i].second)});

      children.push_back({{}, mtkv(false, grandchildren)});
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

  /** Renumbers subtrees recursively.
   *  Helper functions to deal with recursion.
   *  Recursive function. */
  KVTreePv2<Node> renumber_children(const KVTreePv2<Node> t) {

    if (renumber_children_cache.find(t) == renumber_children_cache.end()) {
      decltype(KVTree<bool>::children) children;
      auto tchildren = t->children;
      children.reserve(tchildren.size());
      for (auto i = 0; i != tchildren.size(); ++i) {
        children.push_back({{i}, renumber_children(tchildren[i].second)});
      }
      auto res = mtkv(t->n, children);
      renumber_children_cache[t] = res;
      renumber_children_cache[res] = res; // It is idempotent
    }
    return renumber_children_cache[t];
  }
  // Caches for memoisation
  std::map<KVTreePv2<Node>, KVTreePv2<Node>> renumber_children_cache;

  std::map<std::tuple<KVTreePv2<Node>, // t
                      int,             // level
                      int>,            // nparts
           KVTreePv2<Node>>
      q_cache;

  std::map<std::tuple<KVTreePv2<Node>, // t
                      int,             // level
                      int>,            // halosize
           KVTreePv2<Node>>
      bb_cache;

  std::map<std::tuple<KVTreePv2<Node>, // t
                      int,             // levelstart
                      int>,            // levelend
           KVTreePv2<Node>>
      flatten_cache;

  std::map<std::tuple<KVTreePv2<Node>, // t
                      int>,            // level
           KVTreePv2<Node>>
      eo_naive_cache;

  std::map<std::tuple<KVTreePv2<Node>, // t
                      int,             // level
                      vector<int>>,    // index_map
           KVTreePv2<Node>>
      remap_level_cache;

  std::map<std::tuple<KVTreePv2<Node>, // t
                      vector<int>>,    // new_level_ordering
           KVTreePv2<Node>>
      swap_levels_cache;

public:
  KVTreePv2<Node> generate_flat_level(int size) {
    auto leaf = mtkv(make_leaf(), {});
    decltype(KVTree<Node>::children) children;
    children.reserve(size);
    for (int i = 0; i < size; ++i) {
      children.push_back({{i}, leaf});
    }
    return mtkv(make_node(), children);
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
    if (q_cache.find({t, level, nparts}) == q_cache.end()) {

      KVTreePv2<Node> res;
      decltype(KVTree<bool>::children) children;
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
      q_cache[{t, level, nparts}] = res;
    }
    return q_cache[{t, level, nparts}];
  }
  /** Splits a level in 3 parts,
   * representing the first border,
   * the bulk and the last border.
   * Affects only level and level+1.
   * Uses recursion. */
  KVTreePv2<Node> bb(KVTreePv2<Node> t, int level, int halosize) {
    if (bb_cache.find({t, level, halosize}) == bb_cache.end()) {

      KVTreePv2<Node> res;
      decltype(KVTree<bool>::children) children;
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
      bb_cache[{t, level, halosize}] = res;
    }
    return bb_cache[{t, level, halosize}];
  }
  /** Level collapsing function.
   * Collapses the levels in the range [levelstart,levelend),
   * which are replaced by a single level placed at "levelstart".
   * The keys in the new level are the concatenation
   * of the indices in the collapsed levels.
   * Affects only the levels in said range.
   * Uses recursion. */
  KVTreePv2<Node> flatten(KVTreePv2<Node> t, int levelstart, int levelend) {
    if (flatten_cache.find({t, levelstart, levelend}) == flatten_cache.end()) {

      KVTreePv2<Node> res;
      decltype(KVTree<bool>::children) children;
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
      flatten_cache[{t, levelstart, levelend}] = res;
    }
    return flatten_cache[{t, levelstart, levelend}];
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
    if (eo_naive_cache.find({t, level}) == eo_naive_cache.end()) {

      KVTreePv2<Node> res;
      decltype(KVTree<bool>::children) children;
      if (level == 0) {
        children.reserve(2);
        decltype(children) E, O;
        decltype(children) EO[2] = {E, O};
        for (auto i = 0; i < t->children.size(); ++i) {
          auto keys = t->children[i].first;
          EO[std::accumulate(keys.begin(), keys.end(), 0) % 2].push_back(
              {{i}, t->children[i].second});
        }
        children.push_back({{}, mtkv(false, EO[0])});
        children.push_back({{}, mtkv(false, EO[1])});
        res = mtkv(false, children);
      } else {
        res = renumber_children(t, //
                                [this, level](auto subtree) {
                                  return eo_naive(subtree, level - 1);
                                });
      }
      eo_naive_cache[{t, level}] = res;
    }
    return eo_naive_cache[{t, level}];
  }

  /** Where all these functions transform an input tree into an output tree,
   * this function transforms an index in the opposite direction
   * (that's why it's called 'pullback').
   * */
  /** Reshuffles the keys in a level.*/
  KVTreePv2<Node> remap_level(const KVTreePv2<Node> t, int level,
                              vector<int> index_map) {
    if (remap_level_cache.find({t, level, index_map}) ==
        remap_level_cache.end()) {
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
      remap_level_cache[{t, level, index_map}] = res;
    }
    return remap_level_cache[{t, level, index_map}];
  }

  const KVTreePv2<Node> swap_levels(const KVTreePv2<Node> &tree,
                                    const vector<int> &new_level_ordering) {
    if (swap_levels_cache.find({tree, new_level_ordering}) ==
        swap_levels_cache.end()) {

      KVTreePv2<Node> res;
      if (new_level_ordering.size() == 0)
        res = tree;
      else {
        int next_level = new_level_ordering[0];
        auto new_tree = bring_level_on_top_by_key(tree, next_level);

        auto sub_new_level_ordering = sub_level_ordering(new_level_ordering);

        decltype(tree->children) new_children;
        new_children.reserve(tree->children.size());
        for (const auto &c : new_tree->children) {
          new_children.push_back(
              {c.first, swap_levels(c.second, sub_new_level_ordering)});
        }

        res = mtkv(new_tree->n, new_children);
      }
      swap_levels_cache[{tree, new_level_ordering}] = res;
    }

    return swap_levels_cache[{tree, new_level_ordering}];
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
