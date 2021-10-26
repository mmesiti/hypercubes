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
  int id(TreeTransformerP t);

  std::vector<TreeTransformerP> nodes;

  // Map between names of nodes and actual nodes
  std::map<std::string, TreeTransformerP> named_nodes;

  // the adjacency matrix
  std::map<TreeTransformerP, vector<Arc>> arcs;
};

void Build(TreeFactory<bool> &f,         //
           TransformNetwork &network,    //
           const TransformRequestP root, //
           const vector<TransformRequestP>);

/** Represents a graph of the kind
 *    R
 *  / | \
 *  a b c
 *  \ | /
 *    S
 *  Where S corresponds to a Sum transformer.
 * */
class Sum : public TransformRequest {
private:
  vector<TransformRequestP> elements;
  std::string new_level_name;
  TransformNetwork &network;

public:
  Sum(TransformNetwork &network,  //
      std::string new_level_name, //
      std::string end_node_name,  //
      const vector<TransformRequestP> &elements);
  TreeTransformerP join(TreeFactory<bool> &f,       //
                        TreeTransformerP previous); //
};

/** Represents a graph of the kind
 *    R
 *  / | \
 *  a b c
 *  Where S corresponds to a Sum transformer.
 * */
class Fork : public TransformRequest {
private:
  vector<TransformRequestP> elements;
  TransformNetwork &network;

public:
  Fork(TransformNetwork &network,                  //
       const vector<TransformRequestP> &elements); //

  // This returns NULL
  TreeTransformerP join(TreeFactory<bool> &f,       //
                        TreeTransformerP previous); //
};

/* Represents a graph of the kind
 * R - (a - b - c)
 * Where (a - b - c) corresponds to a Composition transformer
 * */
class Composition : public TransformRequest {
private:
  vector<TransformRequestP> elements;
  TransformNetwork &network;

public:
  Composition(TransformNetwork &network,
              const vector<TransformRequestP> &elements, //
              std::string node_name = "");

  TreeTransformerP join(TreeFactory<bool> &f,       //
                        TreeTransformerP previous); //
};

} // namespace transform_networks
} // namespace internals
} // namespace slow
} // namespace hypercubes

#endif // TRANSFORM_NETWORK_H_
