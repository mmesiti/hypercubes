#ifndef PARTITION_TREE_H_
#define PARTITION_TREE_H_
#include "geometry/geometry.hpp"
#include "selectors/partition_predicates.hpp"
#include "trees/kvtree.hpp"
#include "trees/level_swap.hpp" // TODO: move elsewhere
#include "trees/partition_tree.hpp"

namespace hypercubes {
namespace slow {

class PartitionTree;
class SkeletonTree;
class SizeTree;
class OffsetTree;

class OffsetTree {
private:
  vector<std::string> level_names;
  internals::KVTreeP<int> offset_tree;

  OffsetTree(internals::KVTreeP<int> &&_nct,
             vector<std::string> &&_level_names);

public:
  OffsetTree(const SizeTree &);
  vector<std::string> get_level_names() const;
  // Careful: indices are assumed to be ordered properly.
  int get_offset(const Indices &) const;
  Indices get_indices(int) const;
  // Careful: indices are assumed to be ordered properly.
  OffsetTree get_subtree(const Indices &idxs) const;
  OffsetTree shift(int shift) const;
  const internals::KVTreeP<int> get_internal() const;
};

class SizeTree {
private:
  vector<std::string> level_names;
  internals::KVTreeP<int> size_tree;

  SizeTree(internals::KVTreeP<int> &&_nct, vector<std::string> &&_level_names);

public:
  SizeTree(const SkeletonTree &);
  vector<std::string> get_level_names() const;
  // Careful: indices are assumed to be ordered properly.
  SizeTree get_subtree(const Indices &idxs) const;
  // Careful: indices are assumed to be ordered properly.
  int get_size(const Indices &idxs) const;
  const internals::KVTreeP<int> get_internal() const;
  OffsetTree offset_tree() const;
  friend OffsetTree::OffsetTree(const SizeTree &);
};

class SkeletonTree {
private:
  vector<std::string> level_names;
  internals::KVTreeP<int> skeleton_tree;

  SkeletonTree(internals::KVTreeP<int> &&_nct, vector<std::string> &&_ln);

public:
  SkeletonTree(const PartitionTree &pt);
  vector<std::string> get_level_names() const;
  SkeletonTree permute(const vector<std::string> &_permuted_level_names) const;
  SkeletonTree prune(const PartitionPredicate &) const;
  // Careful: indices are assumed to be ordered properly.
  int get_nchildren(const Indices &) const;
  // Careful: indices are assumed to be ordered properly.
  SkeletonTree get_subtree(const Indices &idxs) const;
  const internals::KVTreeP<int> get_internal() const;
  SizeTree size_tree() const;
  friend SizeTree::SizeTree(const SkeletonTree &);
};

class PartitionTree {
private:
  Sizes sizes;
  PartList partitioners_list;
  internals::PartitionTree partition_tree;

public:
  PartitionTree(Sizes, PartList, vector<int> nonspatial_indices);
  vector<std::string> get_level_names() const;
  Indices get_indices(const Coordinates &) const;
  // TODO: change according to needs
  vector<std::pair<int, Indices>> get_indices_wg(const Coordinates &) const;
  // Careful: indices are assumed to be ordered properly.
  Coordinates get_coordinates(const Indices &) const;
  friend SkeletonTree::SkeletonTree(const PartitionTree &pt);
  SkeletonTree skeleton_tree() const;
  const internals::PartitionTree get_internal() const;
};

// TODO: move elsewhere
template <class TreeIn, class TreeOut> class TreeLevelMatcher {

private:
  vector<int> map;

public:
  TreeLevelMatcher(TreeIn in, TreeOut out)
      : map(internals::find_permutation(in.get_level_names(),
                                        out.get_level_names())) {}
  template <class Container> Container operator()(Container in) {
    Container out(in.size());
    for (int i = 0; i < in.size(); ++i)
      out[map[i]] = in[i];
    return out;
  }
};

template <class TreeIn, class TreeB>
TreeLevelMatcher<TreeIn, TreeB> get_level_matcher(TreeIn in, TreeB out) {
  return TreeLevelMatcher<TreeIn, TreeB>(in, out);
}

} // namespace slow
} // namespace hypercubes

#endif // PARTITION_TREE_H_
