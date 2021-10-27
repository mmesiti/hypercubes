#include "api_v2/transform_network.hpp"
#include "api_v2/transform_requests.hpp"
#include "api_v2/transformer.hpp"
namespace hypercubes {
namespace slow {
namespace internals {
namespace transform_networks {

int TransformNetwork::id(TreeTransformerP t) const {
  return nodes.begin() - std::find(nodes.begin(), //
                                   nodes.end(),   //
                                   t);
}
int TransformNetwork::nnodes() const { return nodes.size(); }

void TransformNetwork::add_node(TreeTransformerP new_node, std::string name) {
  nodes.push_back(new_node);
  // Adding entry for arcs
  arcs[new_node] = {};

  if (name != "") {
    named_nodes[name] = new_node;
  }
  // find node that the new_node points at
  auto source_node =
      *std::find_if(nodes.begin(), //
                    nodes.end(),   //
                    [new_node](auto source_node) {
                      return source_node->output_tree == new_node->input_tree;
                    });
  arcs[source_node].insert({new_node, DIRECT});
  arcs[new_node].insert({source_node, INVERSE});
}

bool operator<(const TransformNetwork::Arc &a, //
               const TransformNetwork::Arc &b) {
  auto ta = std::make_tuple(a.destination, (int)a.type);
  auto tb = std::make_tuple(a.destination, (int)b.type);
  return ta < tb;
}

} // namespace transform_networks
} // namespace internals
} // namespace slow
} // namespace hypercubes
