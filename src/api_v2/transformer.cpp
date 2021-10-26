#include "api_v2/transformer.hpp"
#include "api_v2/tree_transform.hpp"
#include "trees/kvtree_data_structure.hpp"
#include "trees/level_swap.hpp"
#include <stdexcept>

namespace hypercubes {
namespace slow {
namespace internals {
namespace transformers {

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

TreeTransformer::TreeTransformer(TreeTransformerP previous,   //
                                 KVTreePv2<bool> output_tree, //
                                 const vector<std::string> output_levelnames)
    : input_tree(previous->output_tree), //
      output_tree(output_tree),          //
      output_levelnames(output_levelnames) {
  if (not check_names_different())
    throw std::invalid_argument("Level names are not different.");
};

TreeTransformer::TreeTransformer(KVTreePv2<bool> input_output_tree, //
                                 const vector<std::string> output_levelnames)
    : input_tree(input_output_tree),  //
      output_tree(input_output_tree), //
      output_levelnames(output_levelnames) {
  if (not check_names_different())
    throw std::invalid_argument("Level names are not different.");
};

bool TreeTransformer::check_names_different() {
  for (int i = 0; i < output_levelnames.size(); ++i)
    for (int j = i + 1; j < output_levelnames.size(); ++j)
      if (output_levelnames[i] == output_levelnames[j])
        return false;
  return true;
}

vector<Index> Transformer::apply(const Index &in) {
  return index_pushforward(output_tree, in);
}

vector<Index> Transformer::inverse(const Index &in) {
  return vector<Index>{index_pullback(output_tree, in)};
}

Id::Id(TreeFactory<bool> &f, vector<int> dimensions,
       vector<std::string> dimension_names)
    : Transformer(f.generate_nd_tree(dimensions), //
                  dimension_names) {
  if (dimensions.size() != dimension_names.size())
    throw std::invalid_argument("Size of dimensions is not equal"
                                " to size of dimension_names");
}

vector<Index> Id::apply(const Index &idx) { return vector<Index>{idx}; }
vector<Index> Id::inverse(const Index &idx) { return vector<Index>{idx}; }

Q::Q(TreeFactory<bool> &f,      //
     TreeTransformerP previous, //
     std::string level,         //
     int nparts,                //
     std::string name)
    : Transformer(previous,
                  f.q(previous->output_tree,       //
                      previous->find_level(level), //
                      nparts),
                  previous->emplace_name(name, //
                                         level)) {}

BB::BB(TreeFactory<bool> &f,      //
       TreeTransformerP previous, //
       std::string level,         //
       int halosize,              //
       std::string name)
    : Transformer(previous,
                  f.bb(previous->output_tree,       //
                       previous->find_level(level), //
                       halosize),
                  previous->emplace_name(name, //
                                         level)) {}

Flatten::Flatten(TreeFactory<bool> &f,      //
                 TreeTransformerP previous, //
                 std::string level_start,   //
                 std::string level_end,     //
                 std::string name)
    : Transformer(
          previous,
          f.flatten(previous->output_tree,                //
                    previous->find_level(level_start),    //
                    previous->find_level(level_end) + 1), // level_end inclusive
          previous->replace_name_range(name,              //
                                       level_start,       //
                                       level_end)) {}

LevelRemap::LevelRemap(TreeFactory<bool> &f,      //
                       TreeTransformerP previous, //
                       std::string level,         //
                       vector<int> index_map)
    : Transformer(previous,
                  f.remap_level(previous->output_tree,       //
                                previous->find_level(level), //
                                index_map),                  //
                  previous->output_levelnames) {}

Sum::Sum(TreeFactory<bool> &f,                         //
         TreeTransformerP previous,                    //
         const vector<TreeTransformerP> &transformers, //
         std::string name)
    : Transformer(previous, //
                  f.tree_sum([&]() {
                    vector<KVTreePv2<bool>> ts;
                    std::transform(
                        transformers.begin(), //
                        transformers.end(),   //
                        std::back_inserter(ts),
                        [](TreeTransformerP T) { return T->output_tree; });
                    return ts;
                  }()),
                  append(name, previous->output_levelnames)) {
  for (auto T : transformers) {
    if (T->input_tree != previous->output_tree)
      throw std::invalid_argument(
          "All transformers need to have the same input tree,\n"
          "equal to the output of the previous transformer.");
  }
}

LevelSwap::LevelSwap(TreeFactory<bool> &f,      //
                     TreeTransformerP previous, //
                     vector<std::string> names)
    : Transformer(
          previous, //
          f.swap_levels(
              previous->output_tree, //
              [&]() {
                // names must be a permutation of
                // previous->output_levelnames
                if (not check_is_permutation(names,
                                             previous->output_levelnames))
                  throw std::invalid_argument(
                      "New names are not a permutation of the old ones.");

                vector<int> levels;
                levels.reserve(previous->output_levelnames.size());
                std::transform(names.begin(), names.end(),
                               std::back_inserter(levels),
                               [&](const std::string &name) {
                                 return previous->find_level(name);
                               });
                return levels;
              }()),
          names),
      permutation_apply(
          find_permutation(names, previous->output_levelnames)), //
      permutation_inverse(
          find_permutation(previous->output_levelnames, names)) {}

bool LevelSwap::check_is_permutation(vector<std::string> oldnames,
                                     vector<std::string> newnames) {
  return oldnames.size() == newnames.size() and
             [&oldnames, &newnames]() -> bool {
    return std::all_of(oldnames.begin(), oldnames.end(),
                       [&newnames](const std::string &n) {
                         return std::find(newnames.begin(), newnames.end(),
                                          n) != newnames.end();
                       });
  }();
}

vector<Index> LevelSwap::apply(const Index &in) {
  vector<int> out;
  out.reserve(in.size());
  for (int i : permutation_apply)
    out.push_back(in[i]);
  return {out};
}
vector<Index> LevelSwap::inverse(const Index &in) {
  vector<int> out;
  out.reserve(in.size());
  for (int i : permutation_inverse)
    out.push_back(in[i]);
  return {out};
}

EONaive::EONaive(TreeFactory<bool> &f,      //
                 TreeTransformerP previous, //
                 std::string keylevel,      //
                 std::string newname)
    : Transformer(
          previous,
          f.eo_naive(previous->output_tree, previous->find_level(keylevel)),
          previous->emplace_name(newname, keylevel)) {}

} // namespace transformers
} // namespace internals
} // namespace slow
} // namespace hypercubes
