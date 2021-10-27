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
  TreeTransformer(TreeTransformerP previous,   //
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
  virtual vector<Index> apply(const Index &);
  virtual vector<Index> inverse(const Index &);
};
using TransformerP = std::shared_ptr<Transformer>;

struct Id : public Transformer {
public:
  Id(TreeFactory<bool> &f,   //
     vector<int> dimensions, //
     vector<std::string> dimension_names);
  vector<Index> apply(const Index &);
  vector<Index> inverse(const Index &);
};
struct Q : public Transformer {
public:
  Q(TreeFactory<bool> &f,      //
    TreeTransformerP previous, //
    std::string level,         //
    int nparts,                //
    std::string name);
};
struct BB : public Transformer {
public:
  BB(TreeFactory<bool> &f,      //
     TreeTransformerP previous, //
     std::string level,         //
     int halosize,              //
     std::string name);
};
struct Flatten : public Transformer {
  Flatten(TreeFactory<bool> &f,      //
          TreeTransformerP previous, //
          std::string level_start,   //
          std::string level_end,     // INCLUSIVE
          std::string name);
};
struct LevelRemap : public Transformer {
  LevelRemap(TreeFactory<bool> &f,      //
             TreeTransformerP previous, //
             std::string level,         //
             vector<int> index_map);
};
struct Sum : public Transformer {
  Sum(TreeFactory<bool> &f,                         //
      TreeTransformerP previous,                    //
      const vector<TreeTransformerP> &transformers, //
      std::string name);
};

struct LevelSwap : public Transformer {
  LevelSwap(TreeFactory<bool> &f,      //
            TreeTransformerP previous, //
            vector<std::string> names);
  vector<Index> apply(const Index &);
  vector<Index> inverse(const Index &);

private:
  vector<int> permutation_apply;
  vector<int> permutation_inverse;
  static bool check_is_permutation(vector<std::string> oldnames,
                                   vector<std::string> newnames);
};
struct EONaive : public Transformer {
private:
  vector<std::string> levelnames;

public:
  EONaive(TreeFactory<bool> &f,      //
          TreeTransformerP previous, //
          std::string keylevel,      //
          std::string newname);
};

} // namespace transformers
} // namespace internals
} // namespace slow
} // namespace hypercubes

#endif // TRANSFORMER_H_
