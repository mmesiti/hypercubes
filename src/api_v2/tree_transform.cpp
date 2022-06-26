#include "api_v2/tree_transform.hpp"
#include "utils/print_utils.hpp" // DEBUG
#include "utils/utils.hpp"
#include <algorithm>
#include <cmath>
#include <iostream> // DEBUG
#include <numeric>

namespace hypercubes {
namespace slow {
namespace internals {

const int TreeFactory::no_key = -1;
NodeType TreeFactory::make_leaf() { return LEAF; }
NodeType TreeFactory::make_node() { return NODE; }
NodeType TreeFactory::make_ghost() { return GHOST; }

KVTreePv2<NodeType> TreeFactory::tree_product2(const KVTreePv2<NodeType> &t1,
                                               const KVTreePv2<NodeType> &t2) {
  if (t1->n == make_leaf()) {
    return t2;
  } else {
    decltype(KVTree<NodeType>::children) children;
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
vector<int> TreeFactory::sub_level_ordering(const vector<int> &level_ordering) {
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
void TreeFactory::partition_children_into_subtrees(
    decltype(KVTree<NodeType>::children) &children, //
    const vector<int> &starts, // starts do not have to coincide
    const vector<int> &ends,   // with ends.
    const decltype(KVTree<NodeType>::children) &t_children,
    BoundaryCondition bc) {

  children.reserve(ends.size());
  for (int child = 0; child < ends.size(); child++) {
    int i_grandchildren_start = starts[child];
    int i_grandchildren_end = ends[child];
    decltype(KVTree<NodeType>::children) grandchildren;
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
          grandchildren.push_back({{no_key}, mtkv(make_ghost(), {})});
      }
    }

    if (grandchildren.size() > 0)
      children.push_back(
          {{}, // The new level has no correspondence in the old tree
           mtkv(make_node(), grandchildren)});
  }
}
KVTreePv2<NodeType>
TreeFactory::bring_level_on_top(const KVTreePv2<NodeType> tree,
                                int next_level) {

  std::set<vector<int>> keys;
  _collect_keys_at_level(keys, tree, next_level);
  NodeType root;
  { // check on node types - all nodes should be equal.
    std::set<NodeType> nodes;
    _collect_nodes_at_level(nodes, tree, next_level);
    if (nodes.size() != 1)
      _throw_different_nodes_error(nodes, std::string("level:") +
                                              std::to_string(next_level));
    else
      root = *nodes.begin();
  }
  decltype(KVTree<NodeType>::children) children;
  for (auto key : keys) {
    children.push_back({key, collapse_level(tree, next_level, key)});
  }

  return mtkv(root, children);
}
const KVTreePv2<NodeType>                                    //
TreeFactory::collapse_level(const KVTreePv2<NodeType> &tree, //
                            int level_to_collapse,           //
                            const vector<int> &child_key_to_replace) {
  callcounter.total.collapse_level++;
  if (cache.collapse_level.find(
          {tree, level_to_collapse, child_key_to_replace}) ==
      cache.collapse_level.end()) {
    KVTreePv2<NodeType> res = 0; // TODO: are there better ways?
    if (level_to_collapse == 0) {
      for (auto c : tree->children) {
        if (c.first == child_key_to_replace)
          res = c.second;
      }
    } else {
      decltype(KVTree<NodeType>::children) children;
      children.reserve(tree->children.size());
      for (auto ktree : tree->children) {
        const vector<int> k = ktree.first;
        KVTreePv2<NodeType> v = collapse_level(
            ktree.second, level_to_collapse - 1, child_key_to_replace);
        if (v != NULL) // TODO: are there better ways?
          children.push_back({k, v});
      }
      res = mtkv(tree->n, children);
    }
    cache.collapse_level[{tree, level_to_collapse, child_key_to_replace}] = res;
  } else
    callcounter.cached.collapse_level++;
  return cache.collapse_level[{tree, level_to_collapse, child_key_to_replace}];
}

void TreeFactory::print_diagnostics() {
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

KVTreePv2<NodeType> TreeFactory::generate_flat_level(int size) {
  auto leaf = mtkv(make_leaf(), {});
  decltype(KVTree<NodeType>::children) children;
  children.reserve(size);
  for (int i = 0; i < size; ++i) {
    children.push_back({{i}, leaf});
  }
  return mtkv(make_node(), children);
}

KVTreePv2<NodeType>
TreeFactory::renumber_children(const KVTreePv2<NodeType> t) {
  callcounter.total.renumber++;
  if (cache.renumber.find(t) == cache.renumber.end()) {
    decltype(KVTree<NodeType>::children) children;
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
KVTreePv2<NodeType>
TreeFactory::tree_product(const vector<KVTreePv2<NodeType>> &trees) {
  if (trees.size() == 1) {
    return trees[0];
  } else {
    return tree_product2(trees[0], tree_product(tail(trees)));
  }
};

KVTreePv2<NodeType>
TreeFactory::tree_sum(const vector<KVTreePv2<NodeType>> &trees) {
  decltype(KVTree<NodeType>::children) children;
  children.reserve(trees.size());
  for (int i = 0; i < trees.size(); ++i)
    children.push_back({{}, trees[i]});
  return mtkv(make_node(), children);
}

KVTreePv2<NodeType> TreeFactory::generate_nd_tree(std::vector<int> dimensions) {
  vector<KVTreePv2<NodeType>> levels;
  levels.reserve(dimensions.size());
  std::transform(dimensions.begin(),         //
                 dimensions.end(),           //
                 std::back_inserter(levels), //
                 [this](int s) { return generate_flat_level(s); });
  return tree_product(levels);
}

// TODO: existing halo must be zero when periodic,
//       so it makes sense to have two different versions
//       of this function - one with open boundary conditions
//       accepting a "existing halo" parameter,
//       and another with periodic boundary conditions
//       without that parameter
//       (since periodic boundary conditions
//       make sense only for the full lattice,
//       so there is no existing halo)
KVTreePv2<NodeType> TreeFactory::qh(KVTreePv2<NodeType> t, //
                                    int level,             //
                                    int nparts,            //
                                    int halo,              //
                                    int existing_halo,     //
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
  if (t->n == GHOST)
    return t;
  callcounter.total.qh++;
  if (cache.qh.find({t, level, nparts, halo, existing_halo, bc}) ==
      cache.qh.end()) {

    KVTreePv2<NodeType> res;
    decltype(KVTree<NodeType>::children) children;
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
      partition_children_into_subtrees(children, starts, ends, t->children, bc);
      res = mtkv(make_node(), children);
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

KVTreePv2<NodeType> TreeFactory::bb(KVTreePv2<NodeType> t, int level,
                                    int halo) {
  if (t->n == GHOST)
    return t;

  callcounter.total.bb++;
  if (cache.bb.find({t, level, halo}) == cache.bb.end()) {

    KVTreePv2<NodeType> res;
    decltype(KVTree<NodeType>::children) children;
    int size = t->children.size();
    if (level == 0) {
      std::vector<int> limits = {0,           //
                                 halo,        //
                                 size - halo, //
                                 size};
      auto starts = limits; // except the last...
      auto ends = tail(limits);
      partition_children_into_subtrees(children, starts, ends, t->children);
      res = mtkv(make_node(), children);
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
KVTreePv2<NodeType> TreeFactory::hbb(KVTreePv2<NodeType> t, int level,
                                     int halo) {
  if (t->n == GHOST)
    return t;
  callcounter.total.hbb++;
  if (cache.hbb.find({t, level, halo}) == cache.hbb.end()) {

    KVTreePv2<NodeType> res;
    if (level == 0) {
      decltype(KVTree<NodeType>::children) children;
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
      res = mtkv(make_node(), children);
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

KVTreePv2<NodeType> TreeFactory::flatten(KVTreePv2<NodeType> t, //
                                         int levelstart,        //
                                         int levelend) {
  if (t->n == GHOST)
    return t;
  callcounter.total.flatten++;
  if (cache.flatten.find({t, levelstart, levelend}) == cache.flatten.end()) {
    KVTreePv2<NodeType> res;
    decltype(KVTree<NodeType>::children) children;
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
      res = mtkv(make_node(), children);
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

KVTreePv2<NodeType> TreeFactory::collect_leaves(KVTreePv2<NodeType> t, //
                                                int levelstart,        //
                                                int pad_to) {

  if (t->n == GHOST)
    return t;
  callcounter.total.collect_leaves++;
  if (cache.collect_leaves.find({t, levelstart, pad_to}) ==
      cache.collect_leaves.end()) {
    KVTreePv2<NodeType> res;
    decltype(KVTree<NodeType>::children) children;
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
      res = mtkv(make_node(), children);
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

KVTreePv2<NodeType> TreeFactory::eo_naive(const KVTreePv2<NodeType> t,
                                          int level) {
  if (t->n == GHOST)
    return t;
  callcounter.total.eo_naive++;
  if (cache.eo_naive.find({t, level}) == cache.eo_naive.end()) {

    KVTreePv2<NodeType> res;
    decltype(KVTree<NodeType>::children) children;
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
          children.push_back({{}, mtkv(make_node(), eo)});
      }
      res = mtkv(make_node(), children);
    } else {
      res = renumber_children(
          t, //
          [this, level](auto subtree) { return eo_naive(subtree, level - 1); });
    }
    cache.eo_naive[{t, level}] = res;
  } else
    callcounter.cached.eo_naive++;
  return cache.eo_naive[{t, level}];
}

KVTreePv2<NodeType> TreeFactory::remap_level(const KVTreePv2<NodeType> t,
                                             int level, vector<int> index_map) {

  if (t->n == GHOST)
    return t;
  callcounter.total.remap_level++;
  if (cache.remap_level.find({t, level, index_map}) ==
      cache.remap_level.end()) {
    KVTreePv2<NodeType> res;
    decltype(KVTree<NodeType>::children) children;
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

const KVTreePv2<NodeType>
TreeFactory::swap_levels(const KVTreePv2<NodeType> &t,
                         const vector<int> &new_level_ordering) {

  callcounter.total.swap_levels++;
  if (cache.swap_levels.find({t, new_level_ordering}) ==
      cache.swap_levels.end()) {

    KVTreePv2<NodeType> res;
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

} // namespace internals

} // namespace slow
} // namespace hypercubes
