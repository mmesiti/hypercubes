#include "api_v2/transform_request_makers.hpp"

namespace hypercubes {
namespace slow {
namespace internals {
namespace trms { // Transform request makers

TransformRequestP Id(vector<int> dimensions,              //
                     vector<std::string> dimension_names, //
                     std::string end_node_name) {

  return std::make_shared<transform_requests::Id>(dimensions,      //
                                                  dimension_names, //
                                                  end_node_name);
}
TransformRequestP Q(std::string level,          //
                    int nparts,                 //
                    std::string new_level_name, //
                    std::string end_node_name = "") {

  return std::make_shared<transform_requests::Q>(level,          //
                                                 nparts,         //
                                                 new_level_name, //
                                                 end_node_name);
}

TransformRequestP BB(std::string level,          //
                     int halosize,               //
                     std::string new_level_name, //
                     std::string end_node_name){
    return std::make_shared<transform_requests::BB>(level,          //
                                                    halosize,       //
                                                    new_level_name, //
                                                    end_node_name)

}

TransformRequestP Flatten(std::string level_start,    //
                          std::string level_end,      // INCLUSIVE
                          std::string new_level_name, //
                          std::string end_node_name) {
  return std::make_shared<transform_requests::Flatten>(level_start, //
                                                       level_end,   // INCLUSIVE
                                                       new_level_name, //
                                                       end_node_name);
}

TransformRequestP LevelRemap(std::string level,     //
                             vector<int> index_map, //
                             std::string end_node_name) {

  return std::make_shared<transform_requests::LevelRemap>(level,     //
                                                          index_map, //
                                                          end_node_name);
}

TransformRequestP LevelSwap(vector<std::string> names, //
                            std::string end_node_name) {

  return std::make_shared<transform_requests::LevelSwap>(names, //
                                                         end_node_name);
}

TransformRequestP EONaive(std::string keylevel,       //
                          std::string new_level_name, //
                          std::string end_node_name) {
  return std::make_shared<transform_requests::EONaive>(keylevel,       //
                                                       new_level_name, //
                                                       end_node_name);
}

TransformRequestP Sum(const vector<TransformRequestP> &requests, //
                      std::string new_level_name,                //
                      std::string end_node_name) {
  return std::make_shared<transform_requests::Sum>(requests,       //
                                                   new_level_name, //
                                                   end_node_name);
}

TransformRequestP Fork(const vector<TransformRequestP> &requests) {
  return std::make_shared<transform_requests::Fork>(requests);
}
TransformRequestP TreeComposition(const vector<TransformRequestP> &requests) {

  return std::make_shared<transform_requests::TreeComposition>(requests);
}

} // namespace trms
} // namespace internals
} // namespace slow
} // namespace hypercubes
