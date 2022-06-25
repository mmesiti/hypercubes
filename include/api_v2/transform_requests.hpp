#ifndef TRANSFORM_REQUESTS_H_
#define TRANSFORM_REQUESTS_H_
#include "geometry/geometry.hpp"
#include "transformer.hpp"
#include "tree_transform.hpp"
#include <memory>
#include <sstream>
#include <stdexcept>

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
 * as the real constructor needs a TreeFactory object,
 * a "previous" TreeTransformer
 * and in some cases also a reference
 * to the whole transform network.
 */
using transformers::TransformerP;
class TransformRequest { // TODO: it's an abstract class,
                         //       so maybe rename to TransformRequestBase?
protected:
  const std::string end_node_name;

public:
  // the constructor only stores the values
  // that are needed for the Transformer constructor later on.
  TransformRequest(std::string end_node_name);
  // here the constructor of the Transformer will be called
  virtual TransformerP join(TreeFactory &f,        //
                            TransformerP previous, //
                            TransformNetwork &network) const = 0;
  std::string get_end_node_name() const;
};

using TransformRequestP = std::shared_ptr<TransformRequest>;
/* Generic classes - Attempt at reducing the boilerplate. */

template <typename TransformerType>
class TransformRequestGeneric0Arg : public TransformRequest {
private:
public:
  TransformRequestGeneric0Arg(std::string end_node_name = "")
      : TransformRequest(end_node_name){};

  TransformerP join(TreeFactory &f,        //
                    TransformerP previous, //
                    TransformNetwork &_) const {
    try {
      return std::make_shared<TransformerType>(f, previous);
    } catch (std::invalid_argument const &ex) {
      std::stringstream ss;
      ss << "Invalid argument in join:\n" //
         << ex.what() << "\n"             //
         << "end node name: " << end_node_name;
      throw std::invalid_argument(ss.str());
    }
  }
};

template <typename TransformerType, typename... Ts>
class TransformRequestGeneric1Arg : public TransformRequest {
private:
  static_assert(sizeof...(Ts) == 1,
                "This class can be used only with a single parameter.");
  const std::tuple<Ts...> args;

public:
  TransformRequestGeneric1Arg(Ts... args, std::string end_node_name = "")
      : TransformRequest(end_node_name), args(args...){};

  TransformerP join(TreeFactory &f,        //
                    TransformerP previous, //
                    TransformNetwork &_) const {
    try {
      return std::make_shared<TransformerType>(f, previous, //
                                               std::get<0>(args));
    } catch (std::invalid_argument const &ex) {
      std::stringstream ss;
      ss << "Invalid argument in join:\n"              //
         << ex.what() << "\n"                          //
         << "end node name: " << end_node_name << "\n" //
         << "Parameters:" << std::get<0>(args);
      throw std::invalid_argument(ss.str());
    }
  }
};

template <typename TransformerType, typename... Ts>
class TransformRequestGeneric2Arg : public TransformRequest {
private:
  static_assert(sizeof...(Ts) == 2,
                "This class can be used only with 2 parameters.");
  const std::tuple<Ts...> args;

public:
  TransformRequestGeneric2Arg(Ts... args, std::string end_node_name = "")
      : TransformRequest(end_node_name), args(args...){};

  TransformerP join(TreeFactory &f,        //
                    TransformerP previous, //
                    TransformNetwork &_) const {
    try {
      return std::make_shared<TransformerType>(f, previous,       //
                                               std::get<0>(args), //
                                               std::get<1>(args));
    } catch (std::invalid_argument const &ex) {
      std::stringstream ss;
      ss << "Invalid argument in join:\n"              //
         << ex.what() << "\n"                          //
         << "end node name: " << end_node_name << "\n" //
         << "Parameters:" << std::get<0>(args)         //
         << ", " << std::get<1>(args);
      throw std::invalid_argument(ss.str());
    }
  }
};

template <typename TransformerType, typename... Ts>
class TransformRequestGeneric3Arg : public TransformRequest {
private:
  static_assert(sizeof...(Ts) == 3,
                "This class can be used only with 3 parameters.");
  const std::tuple<Ts...> args;

public:
  TransformRequestGeneric3Arg(Ts... args, std::string end_node_name = "")
      : TransformRequest(end_node_name), args(args...){};

  TransformerP join(TreeFactory &f,        //
                    TransformerP previous, //
                    TransformNetwork &_) const {
    try {
      return std::make_shared<TransformerType>(f, previous,       //
                                               std::get<0>(args), //
                                               std::get<1>(args), //
                                               std::get<2>(args));
    } catch (std::invalid_argument const &ex) {
      std::stringstream ss;
      ss << "Invalid argument in join:\n"              //
         << ex.what() << "\n"                          //
         << "end node name: " << end_node_name << "\n" //
         << "Parameters:" << std::get<0>(args)         //
         << ", " << std::get<1>(args)                  //
         << ", " << std::get<2>(args);
      throw std::invalid_argument(ss.str());
    }
  }
};
template <typename TransformerType, typename... Ts>
class TransformRequestGeneric5Arg : public TransformRequest {
private:
  static_assert(sizeof...(Ts) == 5,
                "This class can be used only with 3 parameters.");
  const std::tuple<Ts...> args;

public:
  TransformRequestGeneric5Arg(Ts... args, std::string end_node_name = "")
      : TransformRequest(end_node_name), args(args...){};

  TransformerP join(TreeFactory &f,        //
                    TransformerP previous, //
                    TransformNetwork &_) const {
    try {
      return std::make_shared<TransformerType>(f, previous,       //
                                               std::get<0>(args), //
                                               std::get<1>(args), //
                                               std::get<2>(args), //
                                               std::get<3>(args), //
                                               std::get<4>(args));
    } catch (std::invalid_argument const &ex) {
      std::stringstream ss;
      ss << "Invalid argument in join:\n"              //
         << ex.what() << "\n"                          //
         << "end node name: " << end_node_name << "\n" //
         << "Parameters:" << std::get<0>(args)         //
         << ", " << std::get<1>(args)                  //
         << ", " << std::get<2>(args)                  //
         << ", " << std::get<3>(args)                  //
         << ", " << std::get<4>(args);
      throw std::invalid_argument(ss.str());
    }
  }
};

/* Concrete classes */
class Id : public TransformRequest {
private:
  const vector<int> dimensions;
  const vector<std::string> dimension_names;

public:
  Id(vector<int> dimensions,              //
     vector<std::string> dimension_names, //
     std::string end_node_name = "");
  TransformerP join(TreeFactory &f,             //
                    TransformerP previous,      //
                    TransformNetwork &_) const; // must be 0
};

using Renumber = TransformRequestGeneric0Arg<transformers::Renumber>;

using QFull = TransformRequestGeneric5Arg<transformers::QFull,
                                          std::string,        // level
                                          int,                // nparts
                                          std::string,        // new_level_name
                                          int,                // halo
                                          BoundaryCondition>; // bc

using QSub = TransformRequestGeneric5Arg<transformers::QSub,
                                         std::string, // level
                                         int,         // nparts
                                         std::string, // new_level_name
                                         int,         // halo
                                         int>;        // existing halo

using HBB = TransformRequestGeneric3Arg<transformers::HBB,
                                        std::string,  // level
                                        int,          // halosize
                                        std::string>; // new_level_name

using Flatten = TransformRequestGeneric3Arg<transformers::Flatten,
                                            std::string,  // level_start
                                            std::string,  // level_end
                                            std::string>; // new_level_name

using CollectLeaves = TransformRequestGeneric3Arg<transformers::CollectLeaves,
                                                  std::string, // level_start
                                                  std::string, // new_level_name
                                                  int>;        // pad_to

using LevelRemap = TransformRequestGeneric2Arg<transformers::LevelRemap,
                                               std::string,  // level
                                               vector<int>>; // index_map

using LevelSwap1 =
    TransformRequestGeneric1Arg<transformers::LevelSwap,
                                vector<std::string>>; // level_names
using LevelSwap2 =
    TransformRequestGeneric2Arg<transformers::LevelSwap,
                                vector<std::string>,  // reference_level_names
                                vector<std::string>>; // reordered_level_names

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
  Sum(std::string new_level_name,                //
      const vector<TransformRequestP> &requests, //
      std::string end_node_name = "");
  TransformerP join(TreeFactory &f,                   //
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
  TransformerP join(TreeFactory &f,                   //
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
  TreeComposition(const vector<TransformRequestP> &requests,
                  std::string end_node_name = "");

  // Adds all the subnodes to the network,
  // and return the last node as result.
  // (This is the only object that knows the name of the sub nodes).
  TransformerP join(TreeFactory &f,                   //
                    TransformerP previous,            //
                    TransformNetwork &network) const; //
};

/* This function is the top-level function
 * that takes the list of TransformRequests
 * and builds the actual graph.*/
void Build(TreeFactory &f,            //
           TransformNetwork &network, //
           const vector<TransformRequestP> &requests);

} // namespace transform_requests
} // namespace internals
} // namespace slow
} // namespace hypercubes

#endif // TRANSFORMER_NETWORK_H_
