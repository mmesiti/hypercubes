#include "api_v2/transform_network.hpp"
#include "api_v2/transform_requests.hpp"
#include "api_v2/transformer.hpp"
#include <stdexcept>
namespace hypercubes {
namespace slow {
namespace internals {
namespace transform_networks {

int TransformNetwork::id(TransformerP t) const {
  return nodes.begin() - std::find(nodes.begin(), //
                                   nodes.end(),   //
                                   t);
}
int TransformNetwork::nnodes() const { return nodes.size(); }

bool TransformNetwork::contains_output_tree(
    const TransformerP &new_node) const {
  auto i = std::find_if(nodes.begin(), //
                        nodes.end(),   //
                        [&new_node](const TransformerP &n) {
                          return n->output_tree == new_node->output_tree;
                        });
  return i != nodes.end();
}

void TransformNetwork::add_node(TransformerP new_node, std::string name) {
  if (contains_output_tree(new_node)) {
    throw std::logic_error("Output tree already in network.");
  }

  nodes.push_back(new_node);
  // Adding entry for arcs
  arcs[new_node] = {};

  if (name != "") {
    named_nodes[name] = new_node;
  }
  // find node that the new_node points at
  auto source_node_it =
      std::find_if(nodes.begin(), //
                   nodes.end(),   //
                   [new_node](auto source_node) {
                     return source_node->output_tree == new_node->input_tree;
                   });
  if (source_node_it == nodes.end())
    throw std::domain_error(
        "The source node has not been found in the current network.");
  auto source_node = *source_node_it;
  arcs[source_node].insert({new_node, DIRECT});
  arcs[new_node].insert({source_node, INVERSE});
}

TransformerP TransformNetwork::operator[](const std::string &nodename) {
  return named_nodes[nodename];
};
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
