#ifndef __TOPOSORTING_H_
#define __TOPOSORTING_H_
#include <iostream> //debug
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace hypercubes {
namespace slow {
namespace toposorting {

struct SortablePartitioning {
  using P = std::shared_ptr<SortablePartitioning>;

  std::string id;
  SortablePartitioning(std::string id_) : id(id_) {}
  virtual std::vector<P> children() const = 0;
  bool find_in_children(const SortablePartitioning &other) const {
    bool res = false;
    for (auto c : children()) {
      res = res or c->id == other.id or c->find_in_children(other);
    };
    return res;
  };
  void prepend_children_id(const std::string &prefix) {
    for (auto c : children()) {
      c->id = prefix + '/' + c->id;
      c->prepend_children_id(prefix);
    }
  }
  virtual bool operator<(const SortablePartitioning &other) const = 0;
  virtual std::ostream &stream(std::ostream &os) const {
    return os << "id:" << id << ", ";
  }
};

bool strict_relationship(const SortablePartitioning &partitioning,
                         const SortablePartitioning &other) {
  return not partitioning.find_in_children(other);
}
bool indifferent_relationship(const SortablePartitioning &partitioning,
                              const SortablePartitioning &other) {
  return false;
}

} // namespace toposorting
} // namespace slow
} // namespace hypercubes
#endif
