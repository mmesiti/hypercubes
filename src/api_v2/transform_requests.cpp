#include "api_v2/transform_requests.hpp"
#include <stdexcept>

namespace hypercubes {
namespace slow {
namespace internals {
namespace transform_requests {
TransformRequest::TransformRequest(std::string node_name)
    : node_name(node_name){};

std::string TransformRequest::get_node_name() const { return node_name; }

Id::Id(vector<int> dimensions,              //
       vector<std::string> dimension_names, //
       std::string node_name)
    : TransformRequest(node_name), //
      dimensions(dimensions),      //
      dimension_names(dimension_names) {}

TreeTransformerP Id::join(TreeFactory<bool> &f, TreeTransformerP previous) {

  if (previous)
    throw std::invalid_argument("Id cannot have a previous tree transformer.");

  return std::make_shared<transformers::Id>(f,                //
                                            dimensions,       //
                                            dimension_names); //
}

// must be 0

} // namespace transform_requests
} // namespace internals
} // namespace slow
} // namespace hypercubes
