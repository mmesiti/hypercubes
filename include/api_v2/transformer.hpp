#ifndef TRANSFORMER_H_
#define TRANSFORMER_H_
#include "trees/kvtree_data_structure.hpp"
#include "trees/kvtree_v2.hpp"
#include <memory>
#include <string>
#include <vector>

namespace hypercubes {
namespace slow {
namespace internals {

using std::vector;
using Index = vector<int>;
class IIndexTransformer;
using IndexTransformerP = std::shared_ptr<IIndexTransformer>;
class IIndexTransformer {

public:
  virtual vector<Index> apply(const Index &) = 0;
  virtual vector<Index> inverse(const Index &) = 0;
};

class Inverse : public IIndexTransformer {
private:
  IndexTransformerP wrapped;

public:
  vector<Index> apply(const Index &);
  vector<Index> inverse(const Index &);
  Inverse(const IndexTransformerP);
};

class Composition : public IIndexTransformer {
private:
  const vector<IndexTransformerP> steps;

public:
  vector<Index> apply(const Index &);
  vector<Index> inverse(const Index &);
  Composition(const std::vector<IndexTransformerP>);
};

class TreeTransformer;
using TreeTransformerP = std::shared_ptr<TreeTransformer>;

struct TreeTransformer {

  const KVTreePv2<bool> input_tree;
  const KVTreePv2<bool> output_tree;
  const vector<std::string> output_levelnames;
  vector<std::string> emplace_name(const std::string &name,
                                   const std::string &name_after) const;
  vector<std::string> replace_name_range(const std::string &name,       //
                                         const std::string &name_start, //
                                         const std::string &name_end) const;

  int find_level(const std::string &) const;
  TreeTransformer(TreeTransformerP previous,
                  KVTreePv2<bool> _output_tree, //
                  const vector<std::string> _output_levelnames);
  TreeTransformer(KVTreePv2<bool> input_output_tree, //
                  const vector<std::string> _output_levelnames);
};

class Transformer : public TreeTransformer, public IIndexTransformer {
public:
  template <typename... Args>
  Transformer(Args... args) : TreeTransformer(args...) {}
};

class Id : public Transformer {
public:
  Id(vector<int> dimensions, vector<std::string> dimension_names);
  vector<Index> apply(const Index &);
  vector<Index> inverse(const Index &);
};
class Q : public Transformer {
private:
  int level_idx;

public:
  Q(TreeTransformerP previous, std::string level, int nparts, std::string name);
  vector<Index> apply(const Index &);
  vector<Index> inverse(const Index &);
};
class BB : public Transformer {
private:
  int level_idx;

public:
  BB(TreeTransformerP previous, std::string level, int halosize,
     std::string name);
  vector<Index> apply(const Index &);
  vector<Index> inverse(const Index &);
};
class Flatten : public Transformer {
private:
  int level_start_idx, level_end_idx;

public:
  Flatten(TreeTransformerP previous, //
          std::string level_start,   //
          std::string level_end,     // INCLUSIVE
          std::string name);
  vector<Index> apply(const Index &);
  vector<Index> inverse(const Index &);
};
struct EO : public Transformer {};         // TODO
struct Sum : public Transformer {};        // TODO
struct LevelRemap : public Transformer {}; // TODO
struct LevelSwap : public Transformer {};  // TODO

} // namespace internals
} // namespace slow
} // namespace hypercubes

#endif // TRANSFORMER_H_
