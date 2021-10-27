#ifndef TRANSFORM_REQUESTS_H_
#define TRANSFORM_REQUESTS_H_
#include "transformer.hpp"
#include "tree_transform.hpp"
#include <memory>

namespace hypercubes {
namespace slow {
namespace internals {
namespace transform_networks {
class TransformNetwork;
}
namespace transform_requests {

using transform_networks::TransformNetwork;
/* A lot of boilerplate.
 * All the following classes represent
 * the result of partial function applications
 * of TreeTransformers constructors:
 * they store some arguments
 * (the ones that can be determined by the user)
 * before the real constructor can be called,
 * as the real constructor needs a TreeFactory object
 * and a "previous" TreeTransformer.
 */
using transformers::TransformerP;
class TransformRequest {
protected:
  const std::string end_node_name;

public:
  TransformRequest(std::string end_node_name);
  virtual TransformerP join(TreeFactory<bool> &f,  //
                            TransformerP previous, //
                            TransformNetwork &network) const = 0;
  std::string get_end_node_name() const;
};

using TransformRequestP = std::shared_ptr<TransformRequest>;
/* Attempt at reducing the  */
template <typename TransformerType, typename... Ts>
class TransformRequestGeneric1Arg : public TransformRequest {
private:
  static_assert(sizeof...(Ts) == 1,
                "This class can be used only with a single parameter.");
  const std::tuple<Ts...> args;

public:
  TransformRequestGeneric1Arg(Ts... args, std::string node_name = "")
      : TransformRequest(node_name), args(args...){};

  TransformerP join(TreeFactory<bool> &f,  //
                    TransformerP previous, //
                    TransformNetwork &_) const {
    return std::make_shared<TransformerType>(f, previous, //
                                             std::get<0>(args));
  }
};

template <typename TransformerType, typename... Ts>
class TransformRequestGeneric2Arg : public TransformRequest {
private:
  static_assert(sizeof...(Ts) == 2,
                "This class can be used only with 2 parameters.");
  const std::tuple<Ts...> args;

public:
  TransformRequestGeneric2Arg(Ts... args, std::string node_name = "")
      : TransformRequest(node_name), args(args...){};

  TransformerP join(TreeFactory<bool> &f,  //
                    TransformerP previous, //
                    TransformNetwork &_) const {
    return std::make_shared<TransformerType>(f, previous,       //
                                             std::get<0>(args), //
                                             std::get<1>(args));
  }
};

template <typename TransformerType, typename... Ts>
class TransformRequestGeneric3Arg : public TransformRequest {
private:
  static_assert(sizeof...(Ts) == 3,
                "This class can be used only with 3 parameters.");
  const std::tuple<Ts...> args;

public:
  TransformRequestGeneric3Arg(Ts... args, std::string node_name = "")
      : TransformRequest(node_name), args(args...){};

  TransformerP join(TreeFactory<bool> &f,  //
                    TransformerP previous, //
                    TransformNetwork &_) const {
    return std::make_shared<TransformerType>(f, previous,       //
                                             std::get<0>(args), //
                                             std::get<1>(args), //
                                             std::get<2>(args));
  }
};

class Id : public TransformRequest {
private:
  const vector<int> dimensions;
  const vector<std::string> dimension_names;

public:
  Id(vector<int> dimensions,              //
     vector<std::string> dimension_names, //
     std::string end_node_name = "");
  TransformerP join(TreeFactory<bool> &f,       //
                    TransformerP previous,      //
                    TransformNetwork &_) const; // must be 0
};

using Q = TransformRequestGeneric3Arg<transformers::Q,
                                      std::string,  // level
                                      int,          // nparts
                                      std::string>; // new_level_name

using BB = TransformRequestGeneric3Arg<transformers::BB,
                                       std::string,  // level
                                       int,          // halosize
                                       std::string>; // new_level_name

using Flatten = TransformRequestGeneric3Arg<transformers::Flatten,
                                            std::string,  // level_start
                                            std::string,  // level_end
                                            std::string>; // new_level_name

using LevelRemap = TransformRequestGeneric2Arg<transformers::LevelRemap,
                                               std::string,  // level
                                               vector<int>>; // index_map

using LevelSwap =
    TransformRequestGeneric1Arg<transformers::LevelSwap,
                                vector<std::string>>; // level_names

using EONaive = TransformRequestGeneric2Arg<transformers::EONaive,
                                            std::string,  // keylevel
                                            std::string>; // new_level_name

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
  vector<TransformRequestP> requests;
  std::string new_level_name;

public:
  Sum(std::string new_level_name, //
      std::string end_node_name,  //
      const vector<TransformRequestP> &requests);
  TransformerP join(TreeFactory<bool> &f,             //
                    TransformerP previous,            //
                    TransformNetwork &network) const; //
};

/** Represents a graph of the kind
 *    R
 *  / | \
 *  a b c
 * */
class Fork : public TransformRequest {
private:
  vector<TransformRequestP> requests;

public:
  Fork(const vector<TransformRequestP> &requests); //

  // This returns NULL
  TransformerP join(TreeFactory<bool> &f,             //
                    TransformerP previous,            //
                    TransformNetwork &network) const; //
};

/* Represents a graph of the kind
 * R - (a - b - c)
 * The output transformer is the composition of all the transformers.
 * */
class TreeComposition : public TransformRequest {
private:
  vector<TransformRequestP> requests;

public:
  TreeComposition(const vector<TransformRequestP> &requests);

  TransformerP join(TreeFactory<bool> &f,             //
                    TransformerP previous,            //
                    TransformNetwork &network) const; //
};

void Build(TreeFactory<bool> &f,          //
           TransformNetwork &network,     //
           const TransformRequestP &root, //
           const vector<TransformRequestP> &requests);

} // namespace transform_requests
} // namespace internals
} // namespace slow
} // namespace hypercubes

#endif // TRANSFORMER_NETWORK_H_
