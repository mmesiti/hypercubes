#include "api_v2/transform_network.hpp"
#include "api_v2/transform_requests.hpp"
#include "api_v2/transformer.hpp"
namespace hypercubes {
namespace slow {
namespace internals {
namespace transform_networks {

int TransformNetwork::id(TreeTransformerP t) {
  return nodes.begin() - std::find(nodes.begin(), //
                                   nodes.end(),   //
                                   t);
}

void Build(TreeFactory<bool> &f,                  //
           TransformNetwork &network,             //
           const TransformRequestP &root_request, //
           const vector<TransformRequestP> &requests) {
  TreeTransformerP last = root_request->join(f, 0);
  { // TODO: extract to function
    auto new_node = last;
    auto request = root_request;
    //
    network.nodes.push_back(new_node);
    auto end_node_name = request->get_end_node_name();
    if (end_node_name != "") {
      network.named_nodes[end_node_name] = new_node;
    }
    // Adding entry for arcs
    network.arcs[new_node] = {};
  }
  for (auto request : requests) {
    TreeTransformerP new_node = request->join(f, last);
    {
      // network.nodes
      network.nodes.push_back(new_node);
      // network.named_nodes
      {
        auto end_node_name = request->get_end_node_name();
        if (end_node_name != "") {
          network.named_nodes[end_node_name] = new_node;
        }
      }
      // network.arcs
      // Adding entry to store arcs
      network.arcs[new_node] = {};
      // Adding arc
      network.arcs[last].push_back({new_node, TransformNetwork::DIRECT});
      network.arcs[new_node].push_back({last, TransformNetwork::INVERSE});
    }
    last = new_node;
  }
}

Sum::Sum(TransformNetwork &network,  //
         std::string new_level_name, //
         std::string end_node_name,  //
         const vector<TransformRequestP> &elements)
    : TransformRequest(end_node_name), //
      elements(elements),              //
      new_level_name(new_level_name),  //
      network(network) {}

TreeTransformerP Sum::join(TreeFactory<bool> &f, //
                           TreeTransformerP previous) {
  vector<TreeTransformerP> transformers;

  // Assuming "previous" is already in the network.
  for (auto &request : elements) {
    TreeTransformerP t = request->join(f, previous);
    transformers.push_back(t);

    TreeTransformerP new_node = t;
    TreeTransformerP last = previous;
    { // TODO: extract to function
      // network.nodes
      network.nodes.push_back(new_node);
      // network.named_nodes
      {
        auto end_node_name = request->get_end_node_name();
        if (end_node_name != "") {
          network.named_nodes[end_node_name] = new_node;
        }
      }
      // network.arcs
      // Adding entry to store arcs
      network.arcs[new_node] = {};
      // Adding arc
      network.arcs[last].push_back({new_node, TransformNetwork::DIRECT});
      network.arcs[new_node].push_back({last, TransformNetwork::INVERSE});
    }
  };
  TreeTransformerP res = std::make_shared<transformers::Sum>(f,            //
                                                             previous,     //
                                                             transformers, //
                                                             end_node_name);

  TreeTransformerP new_node = res;
  TreeTransformerP last = previous;
  { // TODO: extract to function
    // network.nodes
    network.nodes.push_back(new_node);
    // network.named_nodes
    {
      if (end_node_name != "") {
        network.named_nodes[end_node_name] = new_node;
      }
    }
    // network.arcs
    // Adding entry to store arcs
    network.arcs[new_node] = {};
    // Adding arc
    network.arcs[last].push_back({new_node, TransformNetwork::DIRECT});
    network.arcs[new_node].push_back({last, TransformNetwork::INVERSE});
  }
}
}

} // namespace transform_networks
} // namespace internals
} // namespace slow
} // namespace hypercubes
