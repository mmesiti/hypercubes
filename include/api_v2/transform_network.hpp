#ifndef TRANSFORM_NETWORK_H_
#define TRANSFORM_NETWORK_H_
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
private:
  // Map between names of nodes and actual nodes
  std::map<std::string, TreeTransformerP> named_nodes;

  // Map containing, for each TreeTransformer,
  // the list of transformers that use it as "previous".
  std::map<TreeTransformerP, vector<TreeTransformerP>> apply_arcs;

  // Map containing, for each TreeTransformer,
  // the "previous" transformer.
  std::map<TreeTransformerP, vector<TreeTransformerP>> inverse_arcs;

public:
};

/* This one is different, as it needs to do some non trivial logic.  */
class Sum : public TransformRequest, public TransformNetwork {
private:
  vector<TransformRequestP> elements;
  std::string new_level_name;

public:
  Sum(const vector<TransformRequestP> &elements, //
      std::string new_level_name,                //
      std::string end_node_name = "");
  TreeTransformerP join(TreeFactory<bool> &f,       //
                        TreeTransformerP previous); //
};

/* This one is different, as it needs to do some non trivial logic.  */
class Composition : public TransformRequest, public TransformNetwork {
private:
  vector<TransformRequestP> elements;

public:
  Composition(const vector<TransformRequestP> &elements, //
              std::string node_name = "");

  TreeTransformerP join(TreeFactory<bool> &f,       //
                        TreeTransformerP previous); //
};

} // namespace transform_networks
} // namespace internals
} // namespace slow
} // namespace hypercubes

#endif // TRANSFORM_NETWORK_H_
