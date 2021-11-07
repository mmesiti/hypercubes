#include "api_v2/transform_request_makers.hpp"
#include "api_v2/transform_requests.hpp"

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

TransformRequestP Renumber(std::string end_node_name) {
  return std::make_shared<transform_requests::Renumber>(end_node_name);
}

TransformRequestP Q(std::string level,          //
                    int nparts,                 //
                    std::string new_level_name, //
                    std::string end_node_name) {

  return std::make_shared<transform_requests::Q>(level,          //
                                                 nparts,         //
                                                 new_level_name, //
                                                 end_node_name);
}

TransformRequestP BB(std::string level,          //
                     int halosize,               //
                     std::string new_level_name, //
                     std::string end_node_name) {
  return std::make_shared<transform_requests::BB>(level,          //
                                                  halosize,       //
                                                  new_level_name, //
                                                  end_node_name);
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

TransformRequestP CollectLeaves(std::string level_start,    //
                                std::string new_level_name, //
                                int pad_to,                 //
                                std::string end_node_name) {
  return std::make_shared<transform_requests::CollectLeaves>(level_start,    //
                                                             new_level_name, //
                                                             pad_to,         //
                                                             end_node_name);
}

TransformRequestP LevelRemap(std::string level,     //
                             vector<int> index_map, //
                             std::string end_node_name) {

  return std::make_shared<transform_requests::LevelRemap>(level,     //
                                                          index_map, //
                                                          end_node_name);
}

TransformRequestP LevelSwap(vector<std::string> level_names, //
                            std::string end_node_name) {

  return std::make_shared<transform_requests::LevelSwap1>(level_names, //
                                                          end_node_name);
}

TransformRequestP LevelSwap(vector<std::string> reference_level_names, //
                            vector<std::string> reordered_level_names, //
                            std::string end_node_name) {

  return std::make_shared<transform_requests::LevelSwap2>(
      reference_level_names, //
      reordered_level_names, //
      end_node_name);
}

TransformRequestP EONaive(std::string keylevel,       //
                          std::string new_level_name, //
                          std::string end_node_name) {
  return std::make_shared<transform_requests::EONaive>(keylevel,       //
                                                       new_level_name, //
                                                       end_node_name);
}

TransformRequestP Sum(std::string new_level_name,                //
                      const vector<TransformRequestP> &requests, //
                      std::string end_node_name) {
  return std::make_shared<transform_requests::Sum>(new_level_name, //
                                                   requests,       //
                                                   end_node_name);
}

TransformRequestP Fork(const vector<TransformRequestP> &requests) {
  return std::make_shared<transform_requests::Fork>(requests);
}
TransformRequestP TreeComposition(const vector<TransformRequestP> &requests,
                                  std::string end_node_name) {

  return std::make_shared<transform_requests::TreeComposition>(requests,
                                                               end_node_name);
}

} // namespace trms
} // namespace internals
} // namespace slow
} // namespace hypercubes
