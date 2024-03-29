#include "api_v2/transform_requests.hpp"
#include "api_v2/transform_network.hpp"
#include "api_v2/transformer.hpp"
#include "api_v2/tree_transform.hpp"
#include <iostream>
#include <stdexcept>

namespace hypercubes {
namespace slow {
namespace internals {
namespace transform_requests {
TransformRequest::TransformRequest(std::string end_node_name)
    : end_node_name(end_node_name){};

std::string TransformRequest::get_end_node_name() const {
  return end_node_name;
}

Id::Id(vector<int> dimensions,              //
       vector<std::string> dimension_names, //
       std::string node_name)
    : TransformRequest(node_name), //
      dimensions(dimensions),      //
      dimension_names(dimension_names) {}

TransformerP Id::join(TreeFactory &f,        //
                      TransformerP previous, //
                      TransformNetwork &_) const {

  if (previous)
    throw std::invalid_argument("Id cannot have a previous tree transformer.");

  return std::make_shared<transformers::Id>(f,                //
                                            dimensions,       //
                                            dimension_names); //
}

EOFix::EOFix(const std::string &level_name,                    //
             const std::string &origin,                        //
             const std::string &previous_name,                 //
             const std::vector<std::string> &levels_reference, //
             std::string end_node_name)
    : TransformRequest(end_node_name), //
      level_name(level_name),          //
      previous_name(previous_name),    //
      origin(origin),                  //
      levels_reference(levels_reference) {}

// TODO: Test
TransformerP EOFix::join(TreeFactory &f,        //
                         TransformerP previous, //
                         TransformNetwork &n) const {

  std::vector<int> levels_reference_int;
  std::transform(levels_reference.begin(),                 //
                 levels_reference.end(),                   //
                 std::back_inserter(levels_reference_int), //
                 [&n, this](const std::string &l) -> int {
                   return n[origin]->find_level(l);
                 });

  // TODO: check that this is the right direction
  auto _transform = n.get_transform(previous_name, origin);
  using Trans = std::function<vector<vector<int>>(const vector<int> &)>;
  Trans transform =
      [&_transform](const vector<int> &idx) -> vector<vector<int>> {
    return _transform->apply(idx); //
  };
  return std::make_shared<transformers::EOFix>(f,          //
                                               previous,   //
                                               level_name, //
                                               transform,  //
                                               levels_reference_int);
}

Sum::Sum(std::string new_level_name,                //
         const vector<TransformRequestP> &requests, //
         std::string end_node_name)
    : TransformRequest(end_node_name), //
      requests(requests),              //
      new_level_name(new_level_name) {}

TransformerP Sum::join(TreeFactory &f,        //
                       TransformerP previous, //
                       TransformNetwork &network) const {
  vector<TransformerP> transformers;

  // Assuming "previous" is already in the network.
  for (auto &request : requests) {
    TransformerP t = request->join(f, previous, network);
    transformers.push_back(t);

    network.add_node(t, request->get_end_node_name());
  };
  TransformerP res = std::make_shared<transformers::Sum>(f,            //
                                                         previous,     //
                                                         transformers, //
                                                         end_node_name);

  // As we add the node to the network for the children,
  // the caller will add this node to the network.
  return res;
}

Fork::Fork(const vector<TransformRequestP> &requests)
    : TransformRequest(""), //
      requests(requests) {}

TransformerP Fork::join(TreeFactory &f,        //
                        TransformerP previous, //
                        TransformNetwork &network) const {

  // Assuming "previous" is already in the network.
  for (auto &request : requests) {
    TransformerP t = request->join(f, previous, network);
    network.add_node(t, request->get_end_node_name());
    std::cout << "FORK:nodes:" << network.nnodes() << std::endl; // DEBUG
    std::cout << "FORK:arcs:" << network.narcs() << std::endl;   // DEBUG
  };

  return 0;
}

TreeComposition::TreeComposition(const vector<TransformRequestP> &requests,
                                 std::string end_node_name)
    : TransformRequest(end_node_name), //
      requests(requests) {}

TransformerP TreeComposition::join(TreeFactory &f,        //
                                   TransformerP previous, //
                                   TransformNetwork &network) const {

  TransformerP last = previous;
  vector<TransformerP> transformers;

  // Assuming "previous" is already in the network.
  for (int i = 0; i < requests.size(); ++i) {
    auto request = requests[i];
    TransformerP t = request->join(f, last, network);
    transformers.push_back(t);

    network.add_node(t, request->get_end_node_name());
    last = t;
    std::cout << end_node_name                             // DEBUG
              << " COMPOSITION:nodes:" << network.nnodes() // DEBUG
              << std::endl;                                // DEBUG
    std::cout << end_node_name                             // DEBUG
              << " COMPOSITION:arcs:" << network.narcs()   // DEBUG
              << std::endl;                                // DEBUG
  };
  return last;
}

void Build(TreeFactory &f,            //
           TransformNetwork &network, //
           const vector<TransformRequestP> &requests) {
  auto root_request = requests[0];
  TransformerP last = root_request->join(f, 0, network);
  network.add_node(last, root_request->get_end_node_name());

  for (auto request_it = requests.begin() + 1; //
       request_it != requests.end();           //
       ++request_it) {
    auto request = *request_it;
    TransformerP new_node = request->join(f, last, network);
    network.add_node(new_node, request->get_end_node_name());
    last = new_node;
  }
}

} // namespace transform_requests
} // namespace internals
} // namespace slow
} // namespace hypercubes
