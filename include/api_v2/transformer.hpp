#ifndef TRANSFORMER_H_
#define TRANSFORMER_H_
#include "tree_transform.hpp"
#include "trees/kvtree_data_structure.hpp"
#include "trees/kvtree_v2.hpp"
#include <memory>
#include <string>
#include <vector>

namespace hypercubes {
namespace slow {
namespace internals {

namespace transformers {

using std::vector;
using Index = vector<int>;
class IIndexTransformer;
using IndexTransformerP = std::shared_ptr<IIndexTransformer>;
class IIndexTransformer {

public:
  virtual vector<Index> apply(const Index &) const = 0;
  virtual vector<Index> inverse(const Index &) const = 0;
};

class Inverse : public IIndexTransformer {
private:
  IndexTransformerP wrapped;

public:
  vector<Index> apply(const Index &) const;
  vector<Index> inverse(const Index &) const;
  Inverse(const IndexTransformerP);
};

class Composition : public IIndexTransformer {
private:
  const vector<IndexTransformerP> steps;

public:
  vector<Index> apply(const Index &) const;
  vector<Index> inverse(const Index &) const;
  Composition(const std::vector<IndexTransformerP>);
};

class Transformer;
using TransformerP = std::shared_ptr<Transformer>;
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
  TreeTransformer(TransformerP previous,       //
                  KVTreePv2<bool> output_tree, //
                  const vector<std::string> output_levelnames);
  TreeTransformer(KVTreePv2<bool> input_output_tree, //
                  const vector<std::string> output_levelnames);

private:
  bool check_names_different();
};

class Transformer : public TreeTransformer, public IIndexTransformer {
public:
  template <typename... Args>
  Transformer(Args... args) : TreeTransformer(args...) {}
  virtual vector<Index> apply(const Index &) const;
  virtual vector<Index> inverse(const Index &) const;
};
using TransformerP = std::shared_ptr<Transformer>;

struct Id : public Transformer {
public:
  Id(TreeFactory<bool> &f,   //
     vector<int> dimensions, //
     vector<std::string> dimension_names);
  vector<Index> apply(const Index &) const;
  vector<Index> inverse(const Index &) const;
};

struct Renumber : public Transformer{      
  public:
    Renumber(TreeFactory<bool> &f,
             TransformerP previous);
};

struct Q : public Transformer {
public:
  Q(TreeFactory<bool> &f,  //
    TransformerP previous, //
    std::string level,     //
    int nparts,            //
    std::string new_level_name);
};
struct BB : public Transformer {
public:
  BB(TreeFactory<bool> &f,  //
     TransformerP previous, //
     std::string level,     //
     int halosize,          //
     std::string new_level_name);
};
struct Flatten : public Transformer {
  Flatten(TreeFactory<bool> &f,    //
          TransformerP previous,   //
          std::string level_start, //
          std::string level_end,   // INCLUSIVE
          std::string new_level_name);
};
struct LevelRemap : public Transformer {
  LevelRemap(TreeFactory<bool> &f,  //
             TransformerP previous, //
             std::string level,     //
             vector<int> index_map);
};
struct Sum : public Transformer {
  Sum(TreeFactory<bool> &f,                     //
      TransformerP previous,                    //
      const vector<TransformerP> &transformers, //
      std::string new_level_name);
};
class TreeComposition : public Transformer {
private:
  Composition composition;

public:
  TreeComposition(TreeFactory<bool> &f,  //
                  TransformerP previous, //
                  const vector<TransformerP> &transformers);
  vector<Index> apply(const Index &) const;
  vector<Index> inverse(const Index &) const;
};
struct LevelSwap : public Transformer {
  LevelSwap(TreeFactory<bool> &f,  //
            TransformerP previous, //
            vector<std::string> level_names);
  LevelSwap(TreeFactory<bool> &f,                      //
            TransformerP previous,                     //
            vector<std::string> reference_level_names, //
            vector<std::string> reordered_level_names);

  vector<Index> apply(const Index &) const;
  vector<Index> inverse(const Index &) const;

private:
  vector<int> permutation_apply;
  vector<int> permutation_inverse;
};
struct EONaive : public Transformer {
private:
  vector<std::string> levelnames;

public:
  EONaive(TreeFactory<bool> &f,  //
          TransformerP previous, //
          std::string keylevel,  //
          std::string new_level_name);
};

} // namespace transformers
} // namespace internals
} // namespace slow
} // namespace hypercubes

#endif // TRANSFORMER_H_
