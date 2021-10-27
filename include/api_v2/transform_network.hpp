#ifndef TRANSFORM_NETWORK_H_
#define TRANSFORM_NETWORK_H_
#include "api_v2/tree_transform.hpp"
#include "transform_requests.hpp"
#include "transformer.hpp"

namespace hypercubes {
namespace slow {
namespace internals {
namespace transform_networks {

using transform_requests::TransformRequest;
using transform_requests::TransformRequestP;
using transformers::TreeTransformerP;

class TransformNetwork {
public:
  enum ArcType { DIRECT, INVERSE };
  struct Arc {
    TreeTransformerP destination;
    ArcType type;
  };

  // returns an integer identifier for a given TreeTransformerP
  // if it is in the network
  int id(TreeTransformerP t) const;
  int nnodes() const;
  void add_node(TreeTransformerP node, std::string name = "");

private:
  std::vector<TreeTransformerP> nodes;

  // Map between names of nodes and actual nodes
  std::map<std::string, TreeTransformerP> named_nodes;

  // the adjacency matrix
  std::map<TreeTransformerP, std::set<Arc>> arcs;
};

bool operator<(const TransformNetwork::Arc &, //
               const TransformNetwork::Arc &);

} // namespace transform_networks
} // namespace internals
} // namespace slow
} // namespace hypercubes

#endif // TRANSFORM_NETWORK_H_
