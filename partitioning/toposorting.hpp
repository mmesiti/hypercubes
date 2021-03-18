#ifndef __TOPOSORTING_H_
#define __TOPOSORTING_H_
#include <memory>
#include <string>
#include <vector>

namespace hypercubes {
namespace slow {
namespace toposorting {

struct SortablePartitioning {
  using P = std::shared_ptr<SortablePartitioning>;

  int id;
  SortablePartitioning(int id_) : id(id_) {}
  virtual std::vector<P> children() const = 0;
  bool find_in_children(const SortablePartitioning &other) const {
    bool res = false;
    for (auto c : children()) {
      res = res or c->id == other.id or c->find_in_children(other);
    };
    return res;
  };
  virtual bool operator<(const SortablePartitioning &other) const {
    return false;
  }
};

struct StrictSortablePartitioning : public SortablePartitioning {
  using SortablePart = SortablePartitioning;
  using P = std::shared_ptr<SortablePart>;

  StrictSortablePartitioning(int id) : SortablePart(id) {}
  virtual std::vector<P> children() const = 0;
  bool operator<(const SortablePart &other) const {
    return not find_in_children(other);
  };
};

} // namespace toposorting
} // namespace slow
} // namespace hypercubes
#endif
