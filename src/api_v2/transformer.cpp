#include "api_v2/transformer.hpp"
#include "api_v2/tree_transform.hpp"
#include "trees/kvtree_data_structure.hpp"
#include <stdexcept>

namespace hypercubes {
namespace slow {
namespace internals {

// Inverse
Inverse::Inverse(const IndexTransformerP other) : wrapped(other){};
vector<Index> Inverse::apply(const Index &idx) { return wrapped->inverse(idx); }
vector<Index> Inverse::inverse(const Index &idx) { return wrapped->apply(idx); }
// Product
Composition::Composition(const std::vector<IndexTransformerP> _steps)
    : steps(_steps) {}
vector<Index> Composition::apply(const Index &idx) {
  vector<Index> outs{idx};
  for (const auto &s : steps) {
    vector<Index> tmp;
    for (const auto &out : outs)
      for (const auto &res : s->apply(out))
        tmp.push_back(res);
    outs = tmp;
  }
  return outs;
}
vector<Index> Composition::inverse(const Index &idx) {
  vector<Index> outs{idx};
  for (auto is = steps.rbegin(); is != steps.rend(); ++is) {
    vector<Index> tmp;
    for (const auto &out : outs)
      for (const auto &res : (*is)->inverse(out))
        tmp.push_back(res);
    outs = tmp;
  }
  return outs;
}

int TreeTransformer::find_level(const std::string &key) const {
  return std::find(output_levelnames.begin(), //
                   output_levelnames.end(), key) -
         output_levelnames.begin();
}

vector<std::string>
TreeTransformer::emplace_name(const std::string &name,
                              const std::string &name_after) const {
  vector<std::string> out_names;
  int level = find_level(name_after);
  out_names.reserve(output_levelnames.size() + 1);
  for (int l = 0; l < output_levelnames.size(); ++l) {
    if (l == level)
      out_names.push_back(name);
    out_names.push_back(output_levelnames[l]);
  }
  return out_names;
}

vector<std::string>
TreeTransformer::replace_name_range(const std::string &name,       //
                                    const std::string &name_start, //
                                    const std::string &name_end) const {

  vector<std::string> out_names;
  int level_start = find_level(name_start);
  int level_end = find_level(name_end);
  out_names.reserve(output_levelnames.size() + 1 -
                    (level_end + 1 - level_start));
  for (int l = 0; l < output_levelnames.size(); ++l) {
    if (l == level_start)
      out_names.push_back(name);
    if (l < level_start or level_end < l)
      out_names.push_back(output_levelnames[l]);
  }
  return out_names;
}

TreeTransformer::TreeTransformer(TreeTransformerP previous,    //
                                 KVTreePv2<bool> _output_tree, //
                                 const vector<std::string> _output_levelnames)
    : input_tree(previous->output_tree), //
      output_tree(_output_tree),         //
      output_levelnames(_output_levelnames){};

TreeTransformer::TreeTransformer(KVTreePv2<bool> input_output_tree, //
                                 const vector<std::string> _output_levelnames)
    : input_tree(input_output_tree),  //
      output_tree(input_output_tree), //
      output_levelnames(_output_levelnames){};

Id::Id(vector<int> dimensions, vector<std::string> dimension_names)
    : Transformer(generate_nd_tree(dimensions), //
                  dimension_names) {
  if (dimensions.size() != dimension_names.size())
    throw std::invalid_argument("Size of dimensions is not equal"
                                " to size of dimension_names");
}

vector<Index> Id::apply(const Index &idx) { return vector<Index>{idx}; }
vector<Index> Id::inverse(const Index &idx) { return vector<Index>{idx}; }

Q::Q(TreeTransformerP previous, std::string level, int nparts, std::string name)
    : Transformer(previous,
                  q(previous->output_tree,       //
                    previous->find_level(level), //
                    nparts),
                  previous->emplace_name(name, //
                                         level)) {}

vector<Index> Q::apply(const Index &in) {
  return index_pushforward(output_tree, in);
}
vector<Index> Q::inverse(const Index &in) {
  return vector<Index>{index_pullback(output_tree, in)};
}

BB::BB(TreeTransformerP previous, std::string level, int halosize,
       std::string name)
    : Transformer(previous,
                  bb(previous->output_tree,       //
                     previous->find_level(level), //
                     halosize),
                  previous->emplace_name(name, //
                                         level)) {}

vector<Index> BB::apply(const Index &in) {
  return index_pushforward(output_tree, in);
}
vector<Index> BB::inverse(const Index &in) {
  return vector<Index>{index_pullback(output_tree, in)};
}

Flatten::Flatten(TreeTransformerP previous, //
                 std::string level_start,   //
                 std::string level_end,     //
                 std::string name)
    : Transformer(
          previous,
          flatten(previous->output_tree,                //
                  previous->find_level(level_start),    //
                  previous->find_level(level_end) + 1), // level_end inclusive
          previous->replace_name_range(name,            //
                                       level_start,     //
                                       level_end)) {}

vector<Index> Flatten::apply(const Index &in) {
  return index_pushforward(output_tree, in);
}
vector<Index> Flatten::inverse(const Index &in) {
  return vector<Index>{index_pullback(output_tree, in)};
}

} // namespace internals
} // namespace slow
} // namespace hypercubes
