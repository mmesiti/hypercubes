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
using transformers::IndexTransformerP;
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
  std::vector<Arc> find_transformations(std::string node_name_start,
                                        std::string node_name_end);

  // Returns the transformer or its inverse
  // based on the type of the arc
  static transformers::IndexTransformerP get_transformer(Arc a);

  IndexTransformerP get_transform(const std::string &node_name_start,
                                  const std::string &node_name_end);

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

  std::tuple<bool, std::vector<TransformNetwork::Arc>>
  _find_transform(const TransformerP node_start, //
                  const TransformerP node_end,   //
                  std::vector<TransformerP> visited_nodes);
};

bool operator<(const TransformNetwork::Arc &, //
               const TransformNetwork::Arc &);

} // namespace transform_networks
} // namespace internals
} // namespace slow
} // namespace hypercubes
namespace std {
using TransformNetwork =
    hypercubes::slow::internals::transform_networks::TransformNetwork;
std::ostream &operator<<(std::ostream &os, const TransformNetwork::Arc &a);
} // namespace std

#endif // TRANSFORM_NETWORK_H_
