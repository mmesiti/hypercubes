#include "api_v2/transform_requests.hpp"

namespace hypercubes {
namespace slow {
namespace internals {
namespace transform_requests {
TransformRequest::TransformRequest(std::string node_name)
    : node_name(node_name){};

std::string TransformRequest::get_node_name() const { return node_name; }
} // namespace transform_requests
} // namespace internals
} // namespace slow
} // namespace hypercubes
