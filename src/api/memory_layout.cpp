#include "api/memory_layout.hpp"

using namespace hypercubes::slow;

PartitionTree::PartitionTree(Sizes s, PartList partitioners){

};

vector<Indices> PartitionTree::get_indices(const Coordinates &coords) {
  return vector<Indices>();
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
