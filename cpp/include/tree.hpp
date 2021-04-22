#ifndef TREE_H_
#define TREE_H_
#include <tuple>
#include <vector>

namespace hypercubes {
namespace slow {
template <class Node> using Tree = std::tuple<Node, std::vector<Node>>;
}
} // namespace hypercubes

#endif // TREE_H_
