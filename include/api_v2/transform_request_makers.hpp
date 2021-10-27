#ifndef TRANSFORM_REQUEST_MAKERS_H_
#define TRANSFORM_REQUEST_MAKERS_H_
#include "transform_requests.hpp"

namespace hypercubes {
namespace slow {
namespace internals {
namespace trms { // Transform request makers

// SOOOO much boilerplate code
// TODO: (check that the arguments are in the correct order)
// TODO: write implementation
using transform_requests::TransformRequestP;
TransformRequestP Id(vector<int> dimensions,              //
                     vector<std::string> dimension_names, //
                     std::string end_node_name);
TransformRequestP Q(std::string level,          //
                    int nparts,                 //
                    std::string new_level_name, //
                    std::string end_node_name);
TransformRequestP BB(std::string level,          //
                     int halosize,               //
                     std::string new_level_name, //
                     std::string end_node_name);
TransformRequestP Flatten(std::string level_start,    //
                          std::string level_end,      // INCLUSIVE
                          std::string new_level_name, //
                          std::string end_node_name);
TransformRequestP LevelRemap(std::string level,     //
                             vector<int> index_map, //
                             std::string end_node_name);
TransformRequestP LevelSwap(vector<std::string> names, //
                            std::string end_node_name);
TransformRequestP EONaive(std::string keylevel,       //
                          std::string new_level_name, //
                          std::string end_node_name);
TransformRequestP Sum(const vector<TransformRequestP> &requests, //
                      std::string new_level_name,                //
                      std::string end_node_name);

TransformRequestP Fork(const vector<TransformRequestP> &requests);
TransformRequestP Composition(const vector<TransformRequestP> &requests, //
                              std::string end_node_name);

} // namespace trms

} // namespace internals
} // namespace slow
} // namespace hypercubes

#endif // TRANSFORM_REQUEST_MAKERS_H_
