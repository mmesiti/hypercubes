#ifndef __INDEXER_H_
#define __INDEXER_H_
#include "base.hpp"
#include "hbb.hpp"
#include "leaf.hpp"
#include "qr.hpp"

namespace hypercubes {
namespace slow {
namespace toposorting {

using partitioning1D::BaseP;
using partitioning1D::HBB;
using partitioning1D::Leaf;
using partitioning1D::Q;
using partitioning1D::QR;

struct SortablePartitioning {
  int id;
  SortablePartitioning(int id_) : id(id_) {}
  virtual bool operator<(const SortablePartitioning &other) = 0;
};

struct Indexer {

  int i = 0;
  int id() { return i++; }
};

template <int level>
struct SortableHBB : public HBB<level>, public SortablePartitioning {};
template <int level>
struct SortableQ : public HBB<level>, public SortablePartitioning {};

} // namespace toposorting
} // namespace slow
} // namespace hypercubes
#endif // __INDEXER_H_
