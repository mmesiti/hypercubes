#include "api/memory_layout.hpp"
#include "trees/partition_tree.hpp"

using namespace hypercubes::slow;

PartitionTree::PartitionTree(Sizes _sizes, PartList partitioners,
                             vector<int> nonspatial_dimensions)
    : sizes(_sizes), partitioners_list(partitioners),
      partition_tree(internals::get_partition_treeM( //
          add_parity(_sizes, nonspatial_dimensions), //
          partitioners)){};

Indices PartitionTree::get_indices(const Coordinates &coords) {
  return internals::get_real_indices(partition_tree, coords);
};
vector<std::pair<int, Indices>>
PartitionTree::get_indices_wg(const Coordinates &coords) {

  auto indices_tree =
      internals::get_indices_tree_with_ghosts(partition_tree, coords);
  return internals::get_relevant_indices_flat(indices_tree);
};

Coordinates PartitionTree::get_coordinates(const Indices &idxs) {
  return Coordinates();
}

NChildrenTree::NChildrenTree(const PartitionTree &pt,
                             const PartitionPredicate &) {}
NChildrenTree::NChildrenTree(const NChildrenTree &other,
                             const vector<std::string> &permuted_level_names){};

SizeTree::SizeTree(const NChildrenTree &nchildren_tree) {}

OffsetTree::OffsetTree(const SizeTree &size_tree) {}
int OffsetTree::get_offset(const Indices &idxs) { return 0; }
Indices OffsetTree::get_indices(int offset) { return Indices(); }
