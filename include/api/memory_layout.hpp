#ifndef PARTITION_TREE_H_
#define PARTITION_TREE_H_
#include "geometry/geometry.hpp"
#include "selectors/partition_predicates.hpp"
#include "trees/partition_tree.hpp"

namespace hypercubes {
namespace slow {

class PartitionTree {
private:
  internals::PartitionTree partition_tree;
  PartList partitioners_list;
  Sizes sizes;

public:
  PartitionTree(Sizes, PartList);
  vector<Indices> get_indices(const Coordinates &);
  Coordinates get_coordinates(const Indices &);
};

class NChildrenTree {
private:
  internals::TreeP<std::pair<int, int>> nchildren_tree;
  vector<int> permutation;

public:
  NChildrenTree(const PartitionTree &pt, const PartitionPredicate &);
  NChildrenTree(const NChildrenTree &other,
                const vector<std::string> &permuted_level_names);
};

class SizeTree {
private:
  internals::TreeP<std::pair<int, int>> size_tree;

public:
  SizeTree(const NChildrenTree &);
};

class OffsetTree {
private:
  internals::TreeP<std::pair<int, int>> offset_tree;

public:
  OffsetTree(const SizeTree &);
  int get_offset(const Indices &);
  Indices get_indices(int);
};

} // namespace slow
} // namespace hypercubes

#endif // PARTITION_TREE_H_
