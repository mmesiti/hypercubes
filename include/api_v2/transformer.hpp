#ifndef TRANSFORMER_H_
#define TRANSFORMER_H_
#include "geometry/geometry.hpp"
#include "selectors/selectors.hpp"
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

  const KVTreePv2<NodeType> input_tree;
  const KVTreePv2<NodeType> output_tree;
  const vector<std::string> output_levelnames;
  vector<std::string> emplace_name(const std::string &name,
                                   const std::string &name_after) const;
  vector<std::string> replace_name_range(const std::string &name,       //
                                         const std::string &name_start, //
                                         const std::string &name_end) const;

  // Until the end
  vector<std::string> replace_name_range(const std::string &name, //
                                         const std::string &name_start) const;

  int find_level(const std::string &) const;
  TreeTransformer(TransformerP previous,           //
                  KVTreePv2<NodeType> output_tree, //
                  const vector<std::string> output_levelnames);
  TreeTransformer(KVTreePv2<NodeType> input_output_tree, //
                  const vector<std::string> output_levelnames);

private:
  bool check_names_different();
  vector<std::string> _replace_name_range(const std::string &name, //
                                          int lstart,              //
                                          int lend) const;         // inclusive
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
  Id(TreeFactory &f,         //
     vector<int> dimensions, //
     vector<std::string> dimension_names);
  vector<Index> apply(const Index &) const;
  vector<Index> inverse(const Index &) const;
};

struct Renumber : public Transformer {
public:
  Renumber(TreeFactory &f, TransformerP previous);
};

// TODO: 1. split Q in
//         a. QFull: intended to act on the full lattice,
//           can choose between open and periodic boundary conditions,
//           has no existing halo.
//         b. QSub: uses only open boundary conditions, uses existing halo.
//       2. add halo specification to both.
struct QFull : public Transformer {
public:
  QFull(TreeFactory &f,             //
        TransformerP previous,      //
        std::string level,          //
        int nparts,                 //
        std::string new_level_name, //
        int halo,                   //
        BoundaryCondition bc);

  vector<Index> inverse(const Index &in) const;
};

struct QSub : public Transformer {
public:
  QSub(TreeFactory &f,             //
       TransformerP previous,      //
       std::string level,          //
       int nparts,                 //
       std::string new_level_name, //
       int halo,                   //
       int existing_halo);
};
struct HBB : public Transformer {
public:
  HBB(TreeFactory &f,        //
      TransformerP previous, //
      std::string level,     //
      int halosize,          //
      std::string new_level_name);
};

struct Flatten : public Transformer {
  Flatten(TreeFactory &f,          //
          TransformerP previous,   //
          std::string level_start, //
          std::string level_end,   // INCLUSIVE
          std::string new_level_name);
};
struct CollectLeaves : public Transformer {
  CollectLeaves(TreeFactory &f,             //
                TransformerP previous,      //
                std::string level_start,    //
                std::string new_level_name, //
                int pad_to);

  vector<Index> inverse(const Index &) const;
};
struct LevelRemap : public Transformer {
  LevelRemap(TreeFactory &f,        //
             TransformerP previous, //
             std::string level,     //
             vector<int> index_map);
};
struct Sum : public Transformer {
  Sum(TreeFactory &f,                           //
      TransformerP previous,                    //
      const vector<TransformerP> &transformers, //
      std::string new_level_name);
};
class TreeComposition : public Transformer {
private:
  Composition composition;

public:
  TreeComposition(TreeFactory &f,        //
                  TransformerP previous, //
                  const vector<TransformerP> &transformers);
  vector<Index> apply(const Index &) const;
  vector<Index> inverse(const Index &) const;
};
struct LevelSwap : public Transformer {
  LevelSwap(TreeFactory &f,        //
            TransformerP previous, //
            vector<std::string> level_names);
  LevelSwap(TreeFactory &f,                            //
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
  EONaive(TreeFactory &f,        //
          TransformerP previous, //
          std::string keylevel,  //
          std::string new_level_name);
};

struct Select : public Transformer {
private:
  selectors_v2::Selector s;

public:
  Select(TreeFactory &f,        //
         TransformerP previous, //
         selectors_v2::Selector s);
};
} // namespace transformers
} // namespace internals
} // namespace slow
} // namespace hypercubes

#endif // TRANSFORMER_H_
