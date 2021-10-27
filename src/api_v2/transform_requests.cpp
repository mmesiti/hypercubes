#include "api_v2/transform_requests.hpp"
#include "api_v2/transform_network.hpp"
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

TreeTransformerP Id::join(TreeFactory<bool> &f, //
                          TreeTransformerP previous,
                          TransformNetwork &_) const {

  if (previous)
    throw std::invalid_argument("Id cannot have a previous tree transformer.");

  return std::make_shared<transformers::Id>(f,                //
                                            dimensions,       //
                                            dimension_names); //
}

void Build(TreeFactory<bool> &f,                  //
           TransformNetwork &network,             //
           const TransformRequestP &root_request, //
           const vector<TransformRequestP> &requests) {
  TreeTransformerP last = root_request->join(f, 0, network);
  network.add_node(last, root_request->get_end_node_name());

  for (auto request : requests) {
    TreeTransformerP new_node = request->join(f, last, network);
    network.add_node(new_node, request->get_end_node_name());
    last = new_node;
  }
}

Sum::Sum(std::string new_level_name, //
         std::string end_node_name,  //
         const vector<TransformRequestP> &requests)
    : TransformRequest(end_node_name), //
      requests(requests),              //
      new_level_name(new_level_name) {}

TreeTransformerP Sum::join(TreeFactory<bool> &f, //
                           TreeTransformerP previous,
                           TransformNetwork &network) {
  vector<TreeTransformerP> transformers;

  // Assuming "previous" is already in the network.
  for (auto &request : requests) {
    TreeTransformerP t = request->join(f, previous, network);
    transformers.push_back(t);

    network.add_node(t, request->get_end_node_name());
  };
  TreeTransformerP res = std::make_shared<transformers::Sum>(f,            //
                                                             previous,     //
                                                             transformers, //
                                                             end_node_name);

  network.add_node(res, end_node_name);
  return res;
}

} // namespace transform_requests
} // namespace internals
} // namespace slow
} // namespace hypercubes
