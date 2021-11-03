#include "api_v2/transform_network.hpp"
#include "api_v2/transform_requests.hpp"
#include "api_v2/transformer.hpp"
#include <iostream>
#include <numeric>
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
int TransformNetwork::narcs() const {
  int n = 0;
  for (auto a : arcs)
    n += a.second.size();
  return n;
}

TransformerP TransformNetwork::search_transformers_with_same_output_tree(
    const TransformerP &new_node) const //
{
  auto i = std::find_if(nodes.begin(), //
                        nodes.end(),   //
                        [&new_node](const TransformerP &n) {
                          return n->output_tree == new_node->output_tree;
                        });
  if (i == nodes.end())
    return 0;
  else
    return *i;
}

void TransformNetwork::add_node(TransformerP new_node, std::string name) {
  if (new_node == 0)
    return; // e.g., "Fork" case
  if (find_node(name) != 0)
    throw std::invalid_argument("Name already taken.");
  TransformerP transformer_with_same_output_tree =
      search_transformers_with_same_output_tree(new_node);
  if (transformer_with_same_output_tree) {
    // Then we add the same node with the new name
    if (name != "") {
      named_nodes.push_back({name, transformer_with_same_output_tree});
    }
  } else {
    nodes.push_back(new_node);
    // Adding entry for arcs
    arcs[new_node] = {};

    if (name != "") {
      named_nodes.push_back({name, new_node});
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
}

TransformerP TransformNetwork::find_node(const std::string &nodename) const {
  for (auto &n : named_nodes) {
    if (n.first == nodename)
      return n.second;
  }
  return 0;
}

TransformerP TransformNetwork::operator[](const std::string &nodename) {
  auto res = find_node(nodename);
  if (res)
    return res;
  else
    throw std::invalid_argument("Node not found");
}
vector<std::string> TransformNetwork::operator[](const TransformerP &t) {
  vector<std::string> out;
  for (auto &n : named_nodes) {
    if (n.second == t)
      out.push_back(n.first);
  }
  return out;
}
std::set<std::string> TransformNetwork::nodenames() const {
  std::set<std::string> result;
  for (auto &kv : named_nodes)
    result.insert(kv.first);
  return result;
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
