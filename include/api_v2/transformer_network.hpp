#ifndef TRANSFORMER_NETWORK_H_
#define TRANSFORMER_NETWORK_H_
#include "transformer.hpp"
#include "tree_transform.hpp"
#include <memory>

namespace hypercubes {
namespace slow {
namespace internals {

// A lot of boilerplate.
namespace requests {
using TreeTransformerP = transformers::IndexTransformerP;
class Request {
protected:
  std::string node_name;

public:
  virtual TreeTransformerP join(TreeFactory<bool> &f,
                                TreeTransformerP previous) = 0;
  std::string get_node_name() const;
};
class Id : public Request {
private:
  vector<int> dimensions;
  vector<std::string> dimension_names;

public:
  Id(vector<int> dimensions,              //
     vector<std::string> dimension_names, //
     std::string node_name = "");
  TreeTransformerP join(TreeFactory<bool> &f, //
                        TreeTransformerP _);  // Must be NULL
};

class Q : public Request {
private:
  std::string level; //
  int nparts;        //
  std::string new_level_name;

public:
  Q(std::string level,          //
    int nparts,                 //
    std::string new_level_name, //
    std::string node_name = "");
  TreeTransformerP join(TreeFactory<bool> &f, //
                        TreeTransformerP previous);
};

class BB : public Request {
private:
  std::string level; //
  int halosize;      //
  std::string new_level_name;

public:
  BB(std::string level,          //
     int halosize,               //
     std::string new_level_name, //
     std::string node_name = "");
  TreeTransformerP join(TreeFactory<bool> &f, //
                        TreeTransformerP previous);
};

class Flatten : public Request {
private:
  std::string level_start; //
  std::string level_end;   // INCLUSIVE
  std::string new_level_name;

public:
  Flatten(std::string level_start,    //
          std::string level_end,      // INCLUSIVE
          std::string new_level_name, //
          std::string node_name = "");
  TreeTransformerP join(TreeFactory<bool> &f, //
                        TreeTransformerP previous);
};
class LevelRemap : public Request {
private:
  std::string level;
  vector<int> index_map;

public:
  LevelRemap(std::string level,     //
             vector<int> index_map, //
             std::string node_name = "");

  TreeTransformerP join(TreeFactory<bool> &f, //
                        TreeTransformerP previous);
};

class LevelSwap : public Request {
private:
  vector<std::string> level_names;

public:
  LevelSwap(vector<std::string> level_names, //
            std::string node_name = "");
  TreeTransformerP join(TreeFactory<bool> &f, //
                        TreeTransformerP previous);
};

class EONaive : public Request {
private:
  std::string keylevel;
  std::string new_level_name;

public:
  EONaive(std::string keylevel,       //
          std::string new_level_name, //
          std::string node_name = "");

  TreeTransformerP join(TreeFactory<bool> &f, //
                        TreeTransformerP previous);
};

class Sum : public Request {
private:
  vector<Request> elements;
  std::string new_level_name;

public:
  Sum(const vector<Request> &elements, //
      std::string new_level_name,      //
      std::string node_name = "");
  TreeTransformerP join(TreeFactory<bool> &f,       //
                        TreeTransformerP previous); //
};

class Composition : public Request {
private:
  vector<Request> elements;

public:
  Composition(const vector<Request> &elements, //
              std::string node_name = "");

  TreeTransformerP join(TreeFactory<bool> &f,       //
                        TreeTransformerP previous); //
};

} // namespace requests
} // namespace internals
} // namespace slow
} // namespace hypercubes

#endif // TRANSFORMER_NETWORK_H_
