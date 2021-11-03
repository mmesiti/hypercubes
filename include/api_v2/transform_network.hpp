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
using transformers::TransformerP;

// TODO: test everything
class TransformNetwork {
public:
  enum ArcType { DIRECT, INVERSE };
  struct Arc {
    TransformerP destination;
    ArcType type;
  };

  // returns an integer identifier for a given TransformerP
  // if it is in the network
  int id(TransformerP t) const;
  int nnodes() const;
  int narcs() const;

  void add_node(TransformerP node, std::string name = "");
  TransformerP operator[](const std::string &);
  std::vector<std::string> operator[](const TransformerP &);

  std::set<std::string> nodenames() const;

private:
  TransformerP
  search_transformers_with_same_output_tree(const TransformerP &node) const;
  TransformerP find_node(const std::string &nodename) const;

  std::vector<TransformerP> nodes;

  // Map between names of nodes and actual nodes
  // a single node can have more names.
  // but names must be unique.
  std::vector<std::pair<std::string, TransformerP>> named_nodes;

  // the adjacency matrix
  std::map<TransformerP, std::set<Arc>> arcs;
};

bool operator<(const TransformNetwork::Arc &, //
               const TransformNetwork::Arc &);

} // namespace transform_networks
} // namespace internals
} // namespace slow
} // namespace hypercubes

#endif // TRANSFORM_NETWORK_H_
