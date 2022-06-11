#include "api_v2/transformer.hpp"
#include "api_v2/tree_transform.hpp"
#include "trees/kvtree_data_structure.hpp"
#include "utils/print_utils.hpp"
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <stdexcept>

using namespace hypercubes::slow::internals;
using namespace hypercubes::slow::internals::transformers;

BOOST_AUTO_TEST_SUITE(test_transformers)

BOOST_AUTO_TEST_CASE(test_tree_transformer_find_level) {

  TreeTransformer t{0, {"X", "Y", "Z", "T"}};
  int ilevel = t.find_level("Z");
  BOOST_TEST(ilevel == 2);
}

BOOST_AUTO_TEST_CASE(test_tree_transformer_find_level_notfound_throws) {

  TreeTransformer t{0, {"X", "Y", "Z", "T"}};
  BOOST_CHECK_THROW(t.find_level("Q"), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_tree_transformer_check_names_different) {
  BOOST_CHECK_THROW(TreeTransformer(0, {"X", "Y", "Z", "Z"}),
                    std::invalid_argument);
}
BOOST_AUTO_TEST_CASE(test_tree_transformer_check_names_different_3args) {
  auto t = std::make_shared<Transformer>( //
      (KVTreePv2<bool>)0,                 //
      vector<std::string>{"X", "Y", "Z", "T"});

  BOOST_CHECK_THROW(TreeTransformer(t, //
                                    0, //
                                    {"X", "Y", "Z", "Z"}),
                    std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_tree_transformer_emplace_name) {

  TreeTransformer t{0, {"X", "Y", "Z", "T"}};
  auto new_names = t.emplace_name("MPI Z", "Z");
  vector<std::string> new_names_exp{"X", "Y", "MPI Z", "Z", "T"};
  BOOST_CHECK_EQUAL_COLLECTIONS(new_names.begin(), new_names.end(), //
                                new_names_exp.begin(), new_names_exp.end());
}

BOOST_AUTO_TEST_CASE(test_tree_transformer_replace_name_range) {

  TreeTransformer t{0, {"X", "Y", "Z", "T"}};
  auto new_names = t.replace_name_range("FLAT", "Y", "Z");
  vector<std::string> new_names_exp{"X", "FLAT", "T"};
  BOOST_CHECK_EQUAL_COLLECTIONS(new_names.begin(), new_names.end(), //
                                new_names_exp.begin(), new_names_exp.end());
}
BOOST_AUTO_TEST_CASE(test_id_constructor) {
  TreeFactory<bool> f;
  Id R(f, {3, 4}, {"X", "Y"});
  auto tflat = f.flatten(R.output_tree, 0, 2);
  int nchildren = tflat->children.size();
  BOOST_TEST(nchildren == 12);
}

BOOST_AUTO_TEST_CASE(test_id_constructor_throws) {

  TreeFactory<bool> f;
  BOOST_CHECK_THROW((Id{f, {3, 4, 5}, {"Y", "Z"}}), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_id_apply) {
  TreeFactory<bool> f;
  Id R(f, {3, 4}, {"X", "Y"});
  Index in{1, 2};
  auto out = R.apply(in)[0];
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                in.begin(), in.end());
}

BOOST_AUTO_TEST_CASE(test_id_inverse) {
  TreeFactory<bool> f;
  Id R(f, {3, 4}, {"X", "Y"});
  Index in{1, 2};
  auto out = R.inverse(in)[0];
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                in.begin(), in.end());
}

BOOST_AUTO_TEST_CASE(test_q_constructor) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 4, 4},
                                vector<std::string>{"X", "Y", "Z"});
  Q part(f, R, "Y", 2, "MPI Y");
  vector<std::string> partnames_exp{"X", "MPI Y", "Y", "Z"};
  BOOST_CHECK_EQUAL_COLLECTIONS(partnames_exp.begin(), partnames_exp.end(), //
                                part.output_levelnames.begin(),
                                part.output_levelnames.end());
}

BOOST_AUTO_TEST_CASE(test_q_apply) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f,                    //
                                vector<int>{4, 8, 4}, //
                                vector<std::string>{"X", "Y", "Z"});
  Q part(f, R, "Y", 2, "MPI Y");
  Index out = part.apply({2, 4, 3})[0];
  Index out_exp{2, 1, 0, 3};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}

BOOST_AUTO_TEST_CASE(test_q_inverse) {

  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});
  Q part(f, R, "Y", 2, "MPI Y");
  Index out = part.inverse({2, 1, 0, 3})[0];
  Index out_exp{2, 4, 3};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}

BOOST_AUTO_TEST_CASE(test_bb_constructor) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});
  auto split = std::make_shared<Q>(f, R, "Y", 2, "MPI Y");
  BB part(f, split, "Y", 1, "BB Y");
  vector<std::string> partnames_exp{"X", "MPI Y", "BB Y", "Y", "Z"};
  BOOST_CHECK_EQUAL_COLLECTIONS(partnames_exp.begin(), partnames_exp.end(), //
                                part.output_levelnames.begin(),
                                part.output_levelnames.end());
}

BOOST_AUTO_TEST_CASE(test_bb_apply) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});
  auto split = std::make_shared<Q>(f, R, "Y", 2, "MPI Y");
  BB part(f, split, "Y", 1, "BB Y");
  Index out = part.apply({2, 1, 0, 3})[0];
  Index out_exp{2, 1, 0, 0, 3};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}

BOOST_AUTO_TEST_CASE(test_bb_inverse) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});
  auto split = std::make_shared<Q>(f, R, "Y", 2, "MPI Y");
  BB part(f, split, "Y", 1, "BB Y");
  Index out = part.inverse({2, 1, 0, 0, 3})[0];
  Index out_exp{2, 1, 0, 3};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}

BOOST_AUTO_TEST_CASE(test_composition_apply) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});
  auto split = std::make_shared<Q>(f, R, "Y", 2, "MPI Y");
  auto part = std::make_shared<BB>(f, split, "Y", 1, "BB Y");
  Composition all({split, part});
  Index out = all.apply({2, 5, 3})[0];
  Index out_exp{2, 1, 1, 0, 3};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}

BOOST_AUTO_TEST_CASE(test_tree_composition_apply) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});
  auto split = std::make_shared<Q>(f, R, "Y", 2, "MPI Y");
  auto part = std::make_shared<BB>(f, split, "Y", 1, "BB Y");
  TreeComposition all(f, R, {split, part});
  Index out = all.apply({2, 5, 3})[0];
  Index out_exp{2, 1, 1, 0, 3};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}

BOOST_AUTO_TEST_CASE(test_composition_inverse) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});
  auto split = std::make_shared<Q>(f, R, "Y", 2, "MPI Y");
  auto part = std::make_shared<BB>(f, split, "Y", 1, "BB Y");
  Composition all({split, part});
  Index out = all.inverse({2, 1, 1, 0, 3})[0];
  Index out_exp{2, 5, 3};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}

BOOST_AUTO_TEST_CASE(test_tree_composition_inverse) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});
  auto split = std::make_shared<Q>(f, R, "Y", 2, "MPI Y");
  auto part = std::make_shared<BB>(f, split, "Y", 1, "BB Y");
  TreeComposition all(f, R, {split, part});
  Index out = all.inverse({2, 1, 1, 0, 3})[0];
  Index out_exp{2, 5, 3};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}

BOOST_AUTO_TEST_CASE(test_flatten_constructor) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});
  auto flat = std::make_shared<Flatten>(f, R, "Y", "Z", "FLAT");
  vector<std::string> names_exp{"X", "FLAT"};
  BOOST_CHECK_EQUAL_COLLECTIONS(names_exp.begin(), names_exp.end(), //
                                flat->output_levelnames.begin(),
                                flat->output_levelnames.end());
}

BOOST_AUTO_TEST_CASE(test_flatten_apply) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});
  auto flat = std::make_shared<Flatten>(f, R, "Y", "Z", "FLAT");
  Index out = flat->apply({2, 5, 3})[0];
  Index out_exp{2, 23};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}

BOOST_AUTO_TEST_CASE(test_flatten_inverse) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});
  auto flat = std::make_shared<Flatten>(f, R, "Y", "Z", "FLAT");
  Index out = flat->inverse({2, 23})[0];
  Index out_exp{2, 5, 3};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}

BOOST_AUTO_TEST_CASE(test_collect_leaves_constructor) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f,                 //
                                vector<int>{2, 2}, //
                                vector<std::string>{"X", "Y"});
  auto collected = std::make_shared<CollectLeaves>(f, R, "X", "COLLECTED", 4);
  vector<std::string> names_exp{"COLLECTED"};
  BOOST_CHECK_EQUAL_COLLECTIONS(names_exp.begin(), names_exp.end(),   //
                                collected->output_levelnames.begin(), //
                                collected->output_levelnames.end());
  BOOST_TEST(collected->output_tree->children.size() == 4);
}

BOOST_AUTO_TEST_CASE(test_collect_leaves_constructor_padded) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f,                 //
                                vector<int>{2, 3}, //
                                vector<std::string>{"X", "Y"});
  auto collected = std::make_shared<CollectLeaves>(f, R, "X", "COLLECTED", 8);
  BOOST_TEST(collected->output_tree->children.size() == 8);
}

BOOST_AUTO_TEST_CASE(test_collect_leaves_apply) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f,                 //
                                vector<int>{2, 3}, //
                                vector<std::string>{"X", "Y"});
  auto collected = std::make_shared<CollectLeaves>(f, R, "X", "COLLECTED", 8);
  Index out = collected->apply({1, 1})[0];
  Index out_exp = {4};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}

BOOST_AUTO_TEST_CASE(test_collect_leaves_apply_mid_level) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f,                    //
                                vector<int>{2, 2, 3}, //
                                vector<std::string>{"X", "Y", "Z"});
  auto collected = std::make_shared<CollectLeaves>(f, R, "Y", "COLLECTED", 8);
  Index out = collected->apply({0, 1, 1})[0];
  Index out_exp = {0, 4};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}

BOOST_AUTO_TEST_CASE(test_collect_leaves_inverse) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f,                 //
                                vector<int>{2, 3}, //
                                vector<std::string>{"X", "Y"});
  auto collected = std::make_shared<CollectLeaves>(f, R, "X", "COLLECTED", 8);
  Index out = collected->inverse({4})[0];
  Index out_exp = {1, 1};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}

BOOST_AUTO_TEST_CASE(test_collect_leaves_inverse_midlevel) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f,                    //
                                vector<int>{2, 2, 3}, //
                                vector<std::string>{"X", "Y", "Z"});
  auto collected = std::make_shared<CollectLeaves>(f, R, "Y", "COLLECTED", 8);
  Index out = collected->inverse({0, 4})[0];
  Index out_exp = {0, 1, 1};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}
BOOST_AUTO_TEST_CASE(test_collect_leaves_inverse_in_pad) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f,                 //
                                vector<int>{2, 3}, //
                                vector<std::string>{"X", "Y"});
  auto collected = std::make_shared<CollectLeaves>(f, R, "X", "COLLECTED", 8);
  auto out = collected->inverse({7});
  BOOST_TEST(out.size() == 0);
}

BOOST_AUTO_TEST_CASE(test_collect_leaves_inverse_in_pad_midlevel) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f,                    //
                                vector<int>{2, 2, 3}, //
                                vector<std::string>{"X", "Y", "Z"});
  auto collected = std::make_shared<CollectLeaves>(f, R, "Y", "COLLECTED", 8);
  auto out = collected->inverse({1, 7});
  BOOST_TEST(out.size() == 0);
}

BOOST_AUTO_TEST_CASE(test_remap_constructor) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});
  auto remapped =
      std::make_shared<LevelRemap>(f, R, "Z", std::vector<int>{0, 1, 2, 1});
  vector<std::string> names_exp{"X", "Y", "Z"};
  BOOST_CHECK_EQUAL_COLLECTIONS(names_exp.begin(), names_exp.end(), //
                                remapped->output_levelnames.begin(),
                                remapped->output_levelnames.end());
}

BOOST_AUTO_TEST_CASE(test_remap_apply_none) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});
  auto remapped =
      std::make_shared<LevelRemap>(f, R, "Z", std::vector<int>{0, 2, 1, 1});
  auto outs = remapped->apply({3, 6, 3});
  BOOST_TEST(outs.size() == 0);
}

BOOST_AUTO_TEST_CASE(test_remap_apply_2) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});
  auto remapped =
      std::make_shared<LevelRemap>(f, R, "Z", std::vector<int>{0, 2, 1, 1});
  auto outs = remapped->apply({3, 6, 1});
  decltype(outs) expouts = {{3, 6, 2}, {3, 6, 3}};
  BOOST_CHECK_EQUAL_COLLECTIONS(outs[0].begin(), outs[0].end(), //
                                expouts[0].begin(), expouts[0].end());
  BOOST_CHECK_EQUAL_COLLECTIONS(outs[1].begin(), outs[1].end(), //
                                expouts[1].begin(), expouts[1].end());
}

BOOST_AUTO_TEST_CASE(test_remap_inverse) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});
  auto remapped =
      std::make_shared<LevelRemap>(f, R, "Z", std::vector<int>{0, 2, 1, 1});
  auto out = remapped->inverse({3, 6, 1})[0];
  decltype(out) expout = {3, 6, 2};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                expout.begin(), expout.end());
}

BOOST_AUTO_TEST_CASE(test_sum_constructor) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f,                 //
                                vector<int>{4, 8}, //
                                vector<std::string>{"X", "Y"});

  auto remapped0 =
      std::make_shared<LevelRemap>(f, R, "X", std::vector<int>{0, 2});
  auto remapped1 =
      std::make_shared<LevelRemap>(f, R, "Y", std::vector<int>{6, 7});
  auto remapped2 =
      std::make_shared<LevelRemap>(f, R, "Y", std::vector<int>{3, 4});

  vector<std::string> output_levelnames{"SUM", "X", "Y"};
  auto sum = std::make_shared<Sum>(f,                              //
                                   R,                              //
                                   vector<TransformerP>{remapped0, //
                                                        remapped1, //
                                                        remapped2},
                                   "SUM");
  BOOST_CHECK_EQUAL_COLLECTIONS(
      sum->output_levelnames.begin(), sum->output_levelnames.end(),
      output_levelnames.begin(), output_levelnames.end());
}
BOOST_AUTO_TEST_CASE(test_sum_constructor_throws) {
  TreeFactory<bool> f;
  auto R0 =
      std::make_shared<Id>(f, vector<int>{4, 8}, vector<std::string>{"X", "Y"});
  auto R1 =
      std::make_shared<Id>(f, vector<int>{4, 8}, vector<std::string>{"X", "Y"});

  auto remapped0 =
      std::make_shared<LevelRemap>(f, R0, "X", std::vector<int>{0, 2});
  auto remapped1 =
      std::make_shared<LevelRemap>(f, R1, "Y", std::vector<int>{6, 7});

  BOOST_CHECK_THROW(Sum(f, R0, {remapped0, remapped1}, "SUM"),
                    std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_sum_apply) {
  TreeFactory<bool> f;
  auto R =
      std::make_shared<Id>(f, vector<int>{4, 8}, vector<std::string>{"X", "Y"});

  auto remapped0 =
      std::make_shared<LevelRemap>(f, R, "X", std::vector<int>{0, 2});
  auto remapped1 =
      std::make_shared<LevelRemap>(f, R, "Y", std::vector<int>{6, 7});
  auto remapped2 =
      std::make_shared<LevelRemap>(f, R, "Y", std::vector<int>{3, 4});

  auto sum = std::make_shared<Sum>(f,                              //
                                   R,                              //
                                   vector<TransformerP>{remapped0, //
                                                        remapped1, //
                                                        remapped2},
                                   "SUM");

  auto outs = sum->apply({0, 3});
  decltype(outs) expouts{{0, 0, 3},  // in remapped0
                         {2, 0, 0}}; // in remapped2
  BOOST_CHECK_EQUAL_COLLECTIONS(outs[0].begin(), outs[0].end(),
                                expouts[0].begin(), expouts[0].end());
  BOOST_CHECK_EQUAL_COLLECTIONS(outs[1].begin(), outs[1].end(),
                                expouts[1].begin(), expouts[1].end());
}

BOOST_AUTO_TEST_CASE(test_sum_inverse) {
  TreeFactory<bool> f;
  auto R =
      std::make_shared<Id>(f, vector<int>{4, 8}, vector<std::string>{"X", "Y"});

  auto remapped0 =
      std::make_shared<LevelRemap>(f, R, "X", std::vector<int>{0, 2});
  auto remapped1 =
      std::make_shared<LevelRemap>(f, R, "Y", std::vector<int>{6, 7});
  auto remapped2 =
      std::make_shared<LevelRemap>(f, R, "Y", std::vector<int>{3, 4});

  auto sum = std::make_shared<Sum>(f,                              //
                                   R,                              //
                                   vector<TransformerP>{remapped0, //
                                                        remapped1, //
                                                        remapped2},
                                   "SUM");

  auto out = sum->inverse({2, 0, 0})[0]; // in remapped2
  decltype(out) expout{0, 3};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), expout.begin(),
                                expout.end());
}

BOOST_AUTO_TEST_CASE(test_levelswap_constructor) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});
  auto swapped =
      std::make_shared<LevelSwap>(f, R, vector<std::string>{"Y", "Z", "X"});
}

BOOST_AUTO_TEST_CASE(test_levelswap_constructor_v2) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{2, 3, 4, 5},
                                vector<std::string>{"X", "Y", "Z", "T"});
  auto swapped1 = std::make_shared<LevelSwap>(
      f, R, vector<std::string>{"X", "Z", "T", "Y"});
  auto swapped2 =
      std::make_shared<LevelSwap>(f, R,                               //
                                  vector<std::string>{"Y", "Z", "T"}, //
                                  vector<std::string>{"Z", "T", "Y"});
  auto levelnames1 = swapped1->output_levelnames;
  auto levelnames2 = swapped2->output_levelnames;
  BOOST_CHECK_EQUAL_COLLECTIONS(levelnames1.begin(), levelnames1.end(), //
                                levelnames2.begin(), levelnames2.end());
  BOOST_TEST(*(swapped1->output_tree) == *(swapped2->output_tree));
}

BOOST_AUTO_TEST_CASE(test_levelswap_constructor_v2_throws_non_existing_level) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 8, 4, 4},
                                vector<std::string>{"X", "Y", "Z", "T"});
  BOOST_CHECK_THROW(
      std::make_shared<LevelSwap>(f, R,                               //
                                  vector<std::string>{"F", "Z", "T"}, //
                                  vector<std::string>{"Z", "T", "F"}),
      std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_levelswap_constructor_v2_throws_wrong_reorder) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 8, 4, 4},
                                vector<std::string>{"X", "Y", "Z", "T"});
  BOOST_CHECK_THROW(
      std::make_shared<LevelSwap>(f, R,                               //
                                  vector<std::string>{"Y", "Z", "T"}, //
                                  vector<std::string>{"Z", "T", "X"}),
      std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_levelswap_check_names_wrong_length) {
  vector<std::string> oldnames{"X", "Y", "Z"};
  vector<std::string> newnames{"X", "Y", "Z", "T"};
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f,                    //
                                vector<int>{4, 8, 4}, //
                                oldnames);

  BOOST_CHECK_THROW(std::make_shared<LevelSwap>(f, R, newnames),
                    std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_levelswap_constructor_throws_too_few_names) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});

  BOOST_CHECK_THROW(
      std::make_shared<LevelSwap>(f, R, vector<std::string>{"Y", "Z"}),
      std::invalid_argument);
}
BOOST_AUTO_TEST_CASE(test_levelswap_constructor_throws_no_permutation) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});

  BOOST_CHECK_THROW(
      std::make_shared<LevelSwap>(f, R, vector<std::string>{"Y", "Z", "T"}),
      std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_levelswap_constructor_complex) {
  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f,                 //)
                                vector<int>{4, 4}, //
                                vector<std::string>{"X", "Y"});
  auto part = std::make_shared<Q>(f, R, "Y", 2, "MPI Y");
  // we need renumbering between Q/BB and LevelSwap
  auto renumbered = std::make_shared<Renumber>(f, part);
  auto swapped =
      std::make_shared<LevelSwap>(f, renumbered, //
                                  vector<std::string>{"MPI Y", "X", "Y"});
  auto leaf = mtkv(true, {});
  auto Y = mtkv(false, {{{0}, leaf}, //
                        {{1}, leaf}});
  auto XY = mtkv(false, {{{0}, Y}, //
                         {{1}, Y}, //
                         {{2}, Y}, //
                         {{3}, Y}});
  auto MPIY_XY = mtkv(false, {{{0}, XY}, //
                              {{1}, XY}});

  BOOST_TEST(*swapped->output_tree == *MPIY_XY);
}

BOOST_AUTO_TEST_CASE(test_levelswap_apply_simple) {

  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});

  auto swapped =
      std::make_shared<LevelSwap>(f, R, vector<std::string>{"Y", "Z", "X"});
  auto out = swapped->apply({1, 2, 3})[0];
  decltype(out) outexp{2, 3, 1};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                outexp.begin(), outexp.end());
}
BOOST_AUTO_TEST_CASE(test_levelswap_inverse_simple) {

  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});

  auto swapped =
      std::make_shared<LevelSwap>(f, R, vector<std::string>{"Y", "Z", "X"});
  auto out = swapped->inverse({2, 3, 1})[0];
  decltype(out) outexp{1, 2, 3};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                outexp.begin(), outexp.end());
}

BOOST_AUTO_TEST_CASE(test_levelswap_apply_afterq) {

  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});

  auto part = std::make_shared<Q>(f, R, "Y", 2, "MPI Y");

  auto swapped = std::make_shared<LevelSwap>(
      f, part, vector<std::string>{"MPI Y", "Z", "Y", "X"});

  Index v0{1, 5, 3};
  Index v1 = part->apply(v0)[0];
  Index v2 = swapped->apply(v1)[0];
  Index v2exp{1,                                      // MPI Y
              3,                                      // Z
              1,                                      // Y
              1};                                     // X
  BOOST_CHECK_EQUAL_COLLECTIONS(v2.begin(), v2.end(), //
                                v2exp.begin(), v2exp.end());
}
BOOST_AUTO_TEST_CASE(test_levelswap_inverse_afterq) {

  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});

  auto swapped =
      std::make_shared<LevelSwap>(f, R, vector<std::string>{"Y", "Z", "X"});
  auto out = swapped->inverse({2, 3, 1})[0];
  decltype(out) outexp{1, 2, 3};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                outexp.begin(), outexp.end());
}

BOOST_AUTO_TEST_CASE(test_eonaive_constructor) {

  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 8, 4, 3},
                                vector<std::string>{"X", "Y", "Z", "ROW"});
  auto flat = std::make_shared<Flatten>(f, R, "X", "Z", "FLAT");
  auto eo = std::make_shared<EONaive>(f, flat, "FLAT", "EO");
  vector<std::string> exp_levelnames{"EO", "FLAT", "ROW"};
  BOOST_CHECK_EQUAL_COLLECTIONS(eo->output_levelnames.begin(), //
                                eo->output_levelnames.end(),   //
                                exp_levelnames.begin(),        //
                                exp_levelnames.end());
}

BOOST_AUTO_TEST_CASE(test_eonaive_apply) {

  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 8, 4, 3},
                                vector<std::string>{"X", "Y", "Z", "ROW"});
  auto flat = std::make_shared<Flatten>(f, R, "X", "Z", "FLAT");
  auto eo = std::make_shared<EONaive>(f, flat, "FLAT", "EO");

  Index in{3, 2, 2, 1};
  auto out1 = flat->apply(in)[0];
  auto out = eo->apply(out1)[0];
  decltype(out) exp_out{1, (3 * 8 * 4 + 2 * 4 + 2) / 2, 1};

  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(),     //
                                out.end(),       //
                                exp_out.begin(), //
                                exp_out.end());
}
BOOST_AUTO_TEST_CASE(test_eonaive_inverse) {

  TreeFactory<bool> f;
  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 8, 4, 3},
                                vector<std::string>{"X", "Y", "Z", "ROW"});
  auto flat = std::make_shared<Flatten>(f, R, "X", "Z", "FLAT");
  auto eo = std::make_shared<EONaive>(f, flat, "FLAT", "EO");

  Index in{1, (3 * 8 * 4 + 2 * 4 + 2) / 2, 1};
  auto out1 = eo->inverse(in)[0];
  auto out = flat->inverse(out1)[0];
  decltype(out) exp_out{3, 2, 2, 1};

  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(),     //
                                out.end(),       //
                                exp_out.begin(), //
                                exp_out.end());
}

BOOST_AUTO_TEST_SUITE_END()
