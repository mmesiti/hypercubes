#include "api_v2/transformer.hpp"
#include "api_v2/tree_transform.hpp"
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <stdexcept>

using namespace hypercubes::slow::internals;

BOOST_AUTO_TEST_SUITE(test_transformers)

BOOST_AUTO_TEST_CASE(test_find_level) {

  TreeTransformer t{0, {"X", "Y", "Z", "T"}};
  int ilevel = t.find_level("Z");
  BOOST_TEST(ilevel == 2);
}

BOOST_AUTO_TEST_CASE(test_emplace_name) {

  TreeTransformer t{0, {"X", "Y", "Z", "T"}};
  auto new_names = t.emplace_name("MPI Z", "Z");
  vector<std::string> new_names_exp{"X", "Y", "MPI Z", "Z", "T"};
  BOOST_CHECK_EQUAL_COLLECTIONS(new_names.begin(), new_names.end(), //
                                new_names_exp.begin(), new_names_exp.end());
}

BOOST_AUTO_TEST_CASE(test_replace_name_range) {

  TreeTransformer t{0, {"X", "Y", "Z", "T"}};
  auto new_names = t.replace_name_range("FLAT", "Y", "Z");
  vector<std::string> new_names_exp{"X", "FLAT", "T"};
  BOOST_CHECK_EQUAL_COLLECTIONS(new_names.begin(), new_names.end(), //
                                new_names_exp.begin(), new_names_exp.end());
}
BOOST_AUTO_TEST_CASE(test_id_constructor) {
  Id R({3, 4}, {"X", "Y"});
  auto tflat = flatten(R.output_tree, 0, 2);
  int nchildren = tflat->children.size();
  BOOST_TEST(nchildren == 12);
}

BOOST_AUTO_TEST_CASE(test_id_constructor_throws) {

  BOOST_CHECK_THROW((Id{{3, 4, 5}, {"Y", "Z"}}), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_id_apply) {
  Id R({3, 4}, {"X", "Y"});
  Index in{1, 2};
  auto out = R.apply(in)[0];
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                in.begin(), in.end());
}

BOOST_AUTO_TEST_CASE(test_id_inverse) {
  Id R({3, 4}, {"X", "Y"});
  Index in{1, 2};
  auto out = R.inverse(in)[0];
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                in.begin(), in.end());
}

BOOST_AUTO_TEST_CASE(test_q_constructor) {
  auto R = std::make_shared<Id>(vector<int>{4, 4, 4},
                                vector<std::string>{"X", "Y", "Z"});
  Q part(R, "Y", 2, "MPI Y");
  vector<std::string> partnames_exp{"X", "MPI Y", "Y", "Z"};
  BOOST_CHECK_EQUAL_COLLECTIONS(partnames_exp.begin(), partnames_exp.end(), //
                                part.output_levelnames.begin(),
                                part.output_levelnames.end());
}

BOOST_AUTO_TEST_CASE(test_q_apply) {
  auto R = std::make_shared<Id>(vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});
  Q part(R, "Y", 2, "MPI Y");
  Index out = part.apply({2, 4, 3})[0];
  Index out_exp{2, 1, 0, 3};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}
BOOST_AUTO_TEST_CASE(test_doubleq) {
  auto R = std::make_shared<Id>(vector<int>{2, 4, 4},
                                vector<std::string>{"X", "Y", "Z"});
  auto partY = std::make_shared<Q>(R, "Y", 2, "MPI Y");
  auto partYZ = std::make_shared<Q>(partY, "Z", 2, "MPI Z");

  auto zsplit = mtkv(false, {{{},
                              mtkv(false, {{{0}, mtkv(true, {})},    //
                                           {{1}, mtkv(true, {})}})}, //
                             {{},
                              mtkv(false, {{{2}, mtkv(true, {})}, //
                                           {{3}, mtkv(true, {})}})}});

  auto ysplit = mtkv(false, {{{},
                              mtkv(false, {{{0}, zsplit},    //
                                           {{1}, zsplit}})}, //
                             {{},
                              mtkv(false, {{{2}, zsplit}, //
                                           {{3}, zsplit}})}});

  auto x = mtkv(false, {{{0}, ysplit}, //
                        {{1}, ysplit}});
  BOOST_TEST(*x == *(partYZ->output_tree));
}

BOOST_AUTO_TEST_CASE(test_q_inverse) {
  auto R = std::make_shared<Id>(vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});
  Q part(R, "Y", 2, "MPI Y");
  Index out = part.inverse({2, 1, 0, 3})[0];
  Index out_exp{2, 4, 3};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}

BOOST_AUTO_TEST_CASE(test_doubleq_apply) {
  auto R = std::make_shared<Id>(vector<int>{2, 4, 4},
                                vector<std::string>{"X", "Y", "Z"});
  auto partY = std::make_shared<Q>(R, "Y", 2, "MPI Y");
  auto partYZ = std::make_shared<Q>(partY, "Z", 2, "MPI Z");
  Index out = partYZ->apply({1, 1, 3})[0];
  Index out_exp{1, 1, 1, 1};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}

BOOST_AUTO_TEST_CASE(test_doubleq_inverse) {
  auto R = std::make_shared<Id>(vector<int>{2, 4, 4},
                                vector<std::string>{"X", "Y", "Z"});
  auto partY = std::make_shared<Q>(R, "Y", 2, "MPI Y");
  auto partYZ = std::make_shared<Q>(partY, "Z", 2, "MPI Z");
  Index out = partYZ->inverse({1, 1, 1, 1})[0];
  Index out_exp{1, 1, 3};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}

BOOST_AUTO_TEST_CASE(test_bb_constructor) {
  auto R = std::make_shared<Id>(vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});
  auto split = std::make_shared<Q>(R, "Y", 2, "MPI Y");
  BB part(split, "Y", 1, "BB Y");
  vector<std::string> partnames_exp{"X", "MPI Y", "BB Y", "Y", "Z"};
  BOOST_CHECK_EQUAL_COLLECTIONS(partnames_exp.begin(), partnames_exp.end(), //
                                part.output_levelnames.begin(),
                                part.output_levelnames.end());
}

BOOST_AUTO_TEST_CASE(test_bb_inverse) {
  auto R = std::make_shared<Id>(vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});
  auto split = std::make_shared<Q>(R, "Y", 2, "MPI Y");
  BB part(split, "Y", 1, "BB Y");
  Index out = part.inverse({2, 1, 0, 0, 3})[0];
  Index out_exp{2, 1, 0, 3};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}

BOOST_AUTO_TEST_CASE(test_bb_apply) {
  auto R = std::make_shared<Id>(vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});
  auto split = std::make_shared<Q>(R, "Y", 2, "MPI Y"); // {4,2,4,4}
  BB part(split, "Y", 1, "BB Y");                       // {4,2,3,[1,2,1],4}
  Index out = part.apply({2, 1, 0, 3})[0];
  Index out_exp{2, 1, 0, 0, 0};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}

BOOST_AUTO_TEST_CASE(test_composition_apply) {
  auto R = std::make_shared<Id>(vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});
  auto split = std::make_shared<Q>(R, "Y", 2, "MPI Y");
  auto part = std::make_shared<BB>(split, "Y", 1, "BB Y");
  Composition all({split, part});
  Index out = all.apply({2, 5, 3})[0];
  Index out_exp{2, 1, 1, 0, 3};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}

BOOST_AUTO_TEST_CASE(test_composition_inverse) {
  auto R = std::make_shared<Id>(vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});
  auto split = std::make_shared<Q>(R, "Y", 2, "MPI Y");
  auto part = std::make_shared<BB>(split, "Y", 1, "BB Y");
  Composition all({split, part});
  Index out = all.inverse({2, 1, 1, 0, 3})[0];
  Index out_exp{2, 5, 3};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}

BOOST_AUTO_TEST_CASE(test_flatten_constructor) {
  auto R = std::make_shared<Id>(vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});
  auto flat = std::make_shared<Flatten>(R, "Y", "Z", "FLAT");
  vector<std::string> names_exp{"X", "FLAT"};
  BOOST_CHECK_EQUAL_COLLECTIONS(names_exp.begin(), names_exp.end(), //
                                flat->output_levelnames.begin(),
                                flat->output_levelnames.end());
}

BOOST_AUTO_TEST_CASE(test_flatten_apply) {
  auto R = std::make_shared<Id>(vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});
  auto flat = std::make_shared<Flatten>(R, "Y", "Z", "FLAT");
  Index out = flat->apply({2, 5, 3})[0];
  Index out_exp{2, 23};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}

BOOST_AUTO_TEST_CASE(test_flatten_inverse) {
  auto R = std::make_shared<Id>(vector<int>{4, 8, 4},
                                vector<std::string>{"X", "Y", "Z"});
  auto flat = std::make_shared<Flatten>(R, "Y", "Z", "FLAT");
  Index out = flat->inverse({2, 23})[0];
  Index out_exp{2, 5, 3};
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                out_exp.begin(), out_exp.end());
}

BOOST_AUTO_TEST_SUITE_END()
