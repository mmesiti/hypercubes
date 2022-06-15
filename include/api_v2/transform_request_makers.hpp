#ifndef TRANSFORM_REQUEST_MAKERS_H_
#define TRANSFORM_REQUEST_MAKERS_H_
#include "transform_requests.hpp"

namespace hypercubes {
namespace slow {
namespace internals {
namespace trms { // Transform request makers

// SOOOO much boilerplate code
using transform_requests::TransformRequestP;
//#define DECLARE_TRANSFORM_REQUEST_MAKER(TransformRequestType)                  \
//  template <typename... Args>                                                  \
//  transform_requests::TransformRequestP TransformRequestType(Args... args) {   \
//    return std::make_shared<transform_requests::TransformRequestType>(         \
//        args...);                                                              \
//  }
//
// DECLARE_TRANSFORM_REQUEST_MAKER(Id)
// DECLARE_TRANSFORM_REQUEST_MAKER(Renumber)
// DECLARE_TRANSFORM_REQUEST_MAKER(Q)
// DECLARE_TRANSFORM_REQUEST_MAKER(BB)
// DECLARE_TRANSFORM_REQUEST_MAKER(Flatten)
// DECLARE_TRANSFORM_REQUEST_MAKER(CollectLeaves)
// DECLARE_TRANSFORM_REQUEST_MAKER(LevelRemap)
// DECLARE_TRANSFORM_REQUEST_MAKER(LevelSwap1)
// DECLARE_TRANSFORM_REQUEST_MAKER(LevelSwap2)
// DECLARE_TRANSFORM_REQUEST_MAKER(EONaive)
// DECLARE_TRANSFORM_REQUEST_MAKER(Sum)
// DECLARE_TRANSFORM_REQUEST_MAKER(Fork)
// DECLARE_TRANSFORM_REQUEST_MAKER(TreeComposition)
//
//#undef DECLARE_TRANSFORM_REQUEST_MAKER
TransformRequestP Id(vector<int> dimensions,              //
                     vector<std::string> dimension_names, //
                     std::string end_node_name = "");

TransformRequestP Renumber(std::string end_node_name = "");

TransformRequestP Q(std::string level,          //
                    int nparts,                 //
                    std::string new_level_name, //
                    std::string end_node_name = "");

TransformRequestP BB(std::string level,          //
                     int halosize,               //
                     std::string new_level_name, //
                     std::string end_node_name = "");

TransformRequestP Flatten(std::string level_start,    //
                          std::string level_end,      // INCLUSIVE
                          std::string new_level_name, //
                          std::string end_node_name = "");

TransformRequestP CollectLeaves(std::string level_start,    //
                                std::string new_level_name, //
                                int pad_to,                 //
                                std::string end_node_name = "");

TransformRequestP LevelRemap(std::string level,     //
                             vector<int> index_map, //
                             std::string end_node_name = "");

TransformRequestP LevelSwap(vector<std::string> level_names, //
                            std::string end_node_name = "");

TransformRequestP LevelSwap(vector<std::string> reference_level_names, //
                            vector<std::string> reordered_level_names, //
                            std::string end_node_name = "");
TransformRequestP EONaive(std::string keylevel,       //
                          std::string new_level_name, //
                          std::string end_node_name = "");

TransformRequestP Sum(std::string new_level_name,                //
                      const vector<TransformRequestP> &requests, //
                      std::string end_node_name = "");

TransformRequestP Fork(const vector<TransformRequestP> &requests);
TransformRequestP TreeComposition(const vector<TransformRequestP> &requests, //
                                  std::string end_node_name = "");

} // namespace trms

} // namespace internals
} // namespace slow
} // namespace hypercubes

#endif // TRANSFORM_REQUEST_MAKERS_H_
