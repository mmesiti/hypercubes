#ifndef PARTITION_TREE_H_
#define PARTITION_TREE_H_
#include "geometry/geometry.hpp"
#include "selectors/partition_predicates.hpp"
#include "trees/kvtree.hpp"
#include "trees/partition_tree.hpp"

namespace hypercubes {
namespace slow {

class PartitionTree;
class NChildrenTree;
class SizeTree;

class OffsetTree {
private:
  internals::KVTreeP<int> offset_tree;

public:
  OffsetTree(const SizeTree &);
  int get_offset(const Indices &) const;
  Indices get_indices(int) const;
  OffsetTree get_subtree(const Indices &idxs) const;
  OffsetTree shift(int shift) const;
  const internals::KVTreeP<int> get_internal() const;
};

class SizeTree {
private:
  internals::KVTreeP<int> size_tree;

public:
  SizeTree(const NChildrenTree &);
  friend OffsetTree::OffsetTree(const SizeTree &);
  SizeTree get_subtree(const Indices &idxs) const;
  const internals::KVTreeP<int> get_internal() const;
};

class NChildrenTree {
private:
  vector<std::string> level_names;
  internals::KVTreeP<int> nchildren_tree;

  NChildrenTree(internals::KVTreeP<int> &&_nct, vector<std::string> &&_ln);

public:
  NChildrenTree(const PartitionTree &pt, const PartitionPredicate &);
  NChildrenTree permute(const vector<std::string> &_permuted_level_names) const;
  friend SizeTree::SizeTree(const NChildrenTree &);
  NChildrenTree get_subtree(const Indices &idxs) const;
  const internals::KVTreeP<int> get_internal() const;
};

class PartitionTree {
private:
  Sizes sizes;
  PartList partitioners_list;
  internals::PartitionTree partition_tree;

public:
  PartitionTree(Sizes, PartList, vector<int> nonspatial_indices);
  Indices get_indices(const Coordinates &);
  // TODO: change according to needs
  vector<std::pair<int, Indices>> get_indices_wg(const Coordinates &);
  Coordinates get_coordinates(const Indices &);
  friend NChildrenTree::NChildrenTree(const PartitionTree &pt,
                                      const PartitionPredicate &);
};

} // namespace slow
} // namespace hypercubes

#endif // PARTITION_TREE_H_
