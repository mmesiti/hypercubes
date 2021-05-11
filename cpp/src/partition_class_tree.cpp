#include "partition_class_tree.hpp"
#include "tree.hpp"
#include <functional>
#include <set>

namespace hypercubes {
namespace slow {
PartitionTree PTBuilder::operator()(SizeParityD sp, //
                                    const PartList &partitioners) {
  Input args{sp, partitioners};
  if (pct_cache.find(args) == pct_cache.end()) {

    PartList other_partitioners;
    std::copy(partitioners.begin() + 1, partitioners.end(),
              std::back_inserter(other_partitioners));

    std::shared_ptr<IPartitioning> n = partitioners[0]->partition(sp);
    vector<PartitionTree> children;
    if (other_partitioners.size() != 0) {
      for (int idx = 0; idx < n->max_idx_value(); ++idx) {
        SizeParitiesD spsD = n->sub_sizeparity_info_list();
        SizeParityD sp_child = spsD[n->idx_to_child_kind(idx)];
        auto new_child = (*this)(sp_child, other_partitioners);
        children.push_back(new_child);
      }
    }
    pct_cache[args] = mt(n, children);
  }
  return pct_cache[args];
}

/**
 * Find first child that represent each class
 * in the children vector.
 * */
vector<int> get_first_class_indices(const PartitionTree &t) {
  int nclasses = t->n->sub_sizeparity_info_list().size();
  vector<int> first_class_indices(nclasses);
  std::fill(first_class_indices.begin(), //
            first_class_indices.end(),   //
            -1);
  for (int idx = 0; idx < t->n->max_idx_value(); ++idx) {
    int class_idx = t->n->idx_to_child_kind(idx);
    if (first_class_indices[class_idx] == -1)
      first_class_indices[class_idx] = idx;
  }
  return first_class_indices;
}

std::string partition_class_tree_to_str(const PartitionTree &t,
                                        const std::string &prefix,
                                        int max_level) {

  std::string new_prefix = prefix + "   ";
  vector<std::string> children_results;
  vector<int> first_class_indices = get_first_class_indices(t);
  if (t->children.size() != 0) {
    for (int i = 0; i < first_class_indices.size() - 1; ++i)
      children_results.push_back(
          partition_class_tree_to_str(t->children[first_class_indices[i]], //
                                      new_prefix + "|",                    //
                                      max_level - 1));
    children_results.push_back(partition_class_tree_to_str(
        t->children[*first_class_indices.rbegin()], //
        new_prefix + " ",                           //
        max_level - 1));
  }
  if (max_level == 0)
    return "";
  std::string res;
  res += prefix + "+" + t->n->get_name() + "\n";
  res += prefix + " " + t->n->comments();
  for (std::string child_result : children_results)
    res += "\n" + child_result;
  return res;
}

TreeP<int> get_indices_tree(const PartitionTree &t, const Coordinates &xs) {
  auto partition_class = t->n;
  vector<IndexResultD> idxs = partition_class->coord_to_idxs(xs);
  IndexResultD real_idr =
      *std::find_if(idxs.begin(), //
                    idxs.end(),   //
                    [](IndexResultD i) { return not i.cached_flag; });

  vector<TreeP<int>> children_results;
  if (t->children.size() != 0) {
    children_results.push_back(
        get_indices_tree(t->children[real_idr.idx], real_idr.rest));
  }
  return mt(real_idr.idx, children_results);
}

TreeP<GhostResult> get_indices_tree_with_ghosts(const PartitionTree &t,
                                                const Coordinates &xs) {

  using F = std::function<TreeP<GhostResult>(int,           //
                                             bool,          //
                                             PartitionTree, //
                                             Coordinates,   //
                                             std::string)>;

  F _get_idtree_wghosts = [&_get_idtree_wghosts](int idx,                //
                                                 bool cached_flag,       //
                                                 const PartitionTree &t, //
                                                 const Coordinates &xs,  //
                                                 const std::string &name) {
    auto partition_class = t->n;
    vector<IndexResultD> idrs = partition_class->coord_to_idxs(xs);
    vector<TreeP<GhostResult>> children_results;
    for (IndexResultD idr : idrs) {
      if (t->children.size() != 0) {
        children_results.push_back(
            _get_idtree_wghosts(idr.idx,              //
                                idr.cached_flag,      //
                                t->children[idr.idx], //
                                idr.rest,             //
                                partition_class->get_name()));
      } else {
        GhostResult gc{idr.idx, idr.cached_flag, partition_class->get_name()};
        children_results.push_back(mt(gc, {}));
      }
    }
    GhostResult g{idx, false, name};
    return mt(g, children_results);
  };

  return _get_idtree_wghosts(0, false, t, xs, "ROOT");
}

vector<std::tuple<int, Indices>>
get_relevant_indices_flat(const TreeP<GhostResult> &tree_indices) {
  int max_depth = get_max_depth(tree_indices);
  auto idxs = get_all_paths(tree_indices);
  using ResType = vector<std::tuple<int, Indices>>;
  decltype(idxs) relevant_idxs;
  // selecting indices that have the correct length
  std::copy_if(idxs.begin(), idxs.end(), std::back_inserter(relevant_idxs),
               [max_depth](auto v) { return v.size() == max_depth; });

  ResType res;

  std::transform(relevant_idxs.begin(), //
                 relevant_idxs.end(),   //
                 std::back_inserter(res), [](const vector<GhostResult> &idx) {
                   int cached_count = std::count_if(
                       idx.begin(), idx.end(),
                       [](GhostResult id) { return id.cached_flag; });
                   Indices i;
                   std::transform(idx.begin(), idx.end(), std::back_inserter(i),
                                  [](GhostResult id) { return id.idx; });
                   return std::make_tuple(cached_count, i);
                 });

  return res;
}

static Indices _shift1(const Indices &idxs) {
  Indices new_idxs;
  std::copy(idxs.begin() + 1, idxs.end(), std::back_inserter(new_idxs));
  return new_idxs;
}

Coordinates get_coord_from_idx(const PartitionTree &t, //
                               const Indices &idxs) {
  int idx = idxs[0];
  Coordinates offsets;
  if (idxs.size() > 1) {
    offsets = get_coord_from_idx(t->children[idx], _shift1(idxs));
  } else {
    offsets = Coordinates(t->n->dimensionality());
    std::fill(offsets.begin(), offsets.end(), 0);
  }
  return t->n->idx_to_coords(idx, offsets);
}

Sizes get_sizes_from_idx(const PartitionTree &t, const Indices &idxs) {
  int idx = idxs[0];
  if (idxs.size() > 1) {
    return get_sizes_from_idx(t->children[idx], _shift1(idxs));
  } else
    return t->n->idx_to_sizes(idx);
}

TreeP<int> get_max_idx_tree(const PartitionTree &t) {
  return nodemap<std::shared_ptr<IPartitioning>, int>(
      t, [](const auto &n) { return n->max_idx_value(); });
}

bool validate_idx(const PartitionTree &t, const Indices &idxs) {
  int idx = idxs[0];
  const auto &node = t->n;
  const auto &children = t->children;
  return (0 <= idx and idx < node->max_idx_value()) //
         and
         (idxs.size() <= 1 //
          or [&]() { return validate_idx(children[idx], _shift1(idxs)); }());
}

vector<std::pair<int, int>> get_partition_limits(const PartitionTree &t, //
                                                 const Indices &idx) {
  Coordinates starts = get_coord_from_idx(t, idx);
  Sizes sizes = get_sizes_from_idx(t, idx);
  vector<std::pair<int, int>> res;
  std::transform(
      starts.begin(), starts.end(), sizes.begin(), //
      std::back_inserter(res),                     //
      [](int start, int size) { return std::make_pair(start, start + size); });
  return res;
}
} // namespace slow
} // namespace hypercubes
