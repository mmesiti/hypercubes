#include "api_v2/transform_network.hpp"
#include "api_v2/transform_requests.hpp"
#include "api_v2/transformer.hpp"
#include "trees/kvtree_data_structure.hpp"
#include "utils/print_utils.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <memory>

using namespace hypercubes::slow::internals;

BOOST_AUTO_TEST_SUITE(test_transform_requests)
using Tree = KVTreePv2<bool>;
using transform_networks::TransformNetwork;

BOOST_AUTO_TEST_CASE(test_id_constructor) {
  TreeFactory<bool> f;
  TransformNetwork n;
  auto idr = transform_requests::Id({2, 2}, {"X", "Y"});
  auto id = idr.join(f, 0, n);

  Tree leaf = mtkv(true, {});
  Tree subtree = mtkv(false, {{{0}, leaf}, //
                              {{1}, leaf}});

  Tree exptree = mtkv(false, {{{0}, subtree}, //
                              {{1}, subtree}});
  BOOST_TEST(*exptree == *(id->output_tree));

  vector<std::string> exp_out_names{"X", "Y"};
  BOOST_CHECK_EQUAL_COLLECTIONS(id->output_levelnames.begin(), //
                                id->output_levelnames.end(),   //
                                exp_out_names.begin(),         //
                                exp_out_names.end());
}

BOOST_AUTO_TEST_CASE(test_id_join_throws) {
  TreeFactory<bool> f;
  TransformNetwork n;
  auto idr = transform_requests::Id({2, 2}, {"X", "Y"});
  auto id = idr.join(f, 0, n);

  BOOST_CHECK_THROW(idr.join(f, id, n), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_q_constructor) {
  TreeFactory<bool> f;
  TransformNetwork n;
  auto idr = transform_requests::Id({2, 4}, {"X", "Y"});
  auto id = idr.join(f, 0, n);

  auto qr = transform_requests::Q("Y", 2, "MPI Y");
  auto q = qr.join(f, id, n);
  Tree leaf = mtkv(true, {});
  Tree subsubtree1 = mtkv(false, {{{0}, leaf}, //
                                  {{1}, leaf}});
  Tree subsubtree2 = mtkv(false, {{{2}, leaf}, //
                                  {{3}, leaf}});

  Tree subtree = mtkv(false, {{{}, subsubtree1}, //
                              {{}, subsubtree2}});
  Tree exptree = mtkv(false, {{{0}, subtree}, //
                              {{1}, subtree}});

  BOOST_TEST(*exptree == *(q->output_tree));
  vector<std::string> exp_out_names{"X", "MPI Y", "Y"};
  BOOST_CHECK_EQUAL_COLLECTIONS(q->output_levelnames.begin(), //
                                q->output_levelnames.end(),   //
                                exp_out_names.begin(),        //
                                exp_out_names.end());
}

BOOST_AUTO_TEST_CASE(test_bb_constructor) {
  TreeFactory<bool> f;
  TransformNetwork n;
  auto idr = transform_requests::Id({2, 4}, {"X", "Y"});
  auto id = idr.join(f, 0, n);

  auto bbr = transform_requests::BB("Y", 1, "BB Y");
  auto bb = bbr.join(f, id, n);
  Tree leaf = mtkv(true, {});

  Tree subtree = mtkv(false, {{{},                          //
                               mtkv(false, {{{0}, leaf}})}, //
                              {{},                          //
                               mtkv(false, {{{1}, leaf},    //
                                            {{2}, leaf}})}, //
                              {{},                          //
                               mtkv(false, {{{3}, leaf}})}});

  Tree exptree = mtkv(false, {{{0}, subtree}, //
                              {{1}, subtree}});

  BOOST_TEST(*exptree == *(bb->output_tree));
  vector<std::string> exp_out_names{"X", "BB Y", "Y"};
  BOOST_CHECK_EQUAL_COLLECTIONS(bb->output_levelnames.begin(), //
                                bb->output_levelnames.end(),   //
                                exp_out_names.begin(),         //
                                exp_out_names.end());
}

BOOST_AUTO_TEST_CASE(test_flatten_constructor) {
  TreeFactory<bool> f;
  TransformNetwork n;
  auto idr = transform_requests::Id({2, 2, 2, 2}, {"X", "Y", "Z", "T"});
  auto id = idr.join(f, 0, n);

  auto flattenr = transform_requests::Flatten("Y", "Z", "YZ");
  auto flatten = flattenr.join(f, id, n);

  Tree leaf = mtkv(true, {});
  Tree T = mtkv(false, {{{0}, leaf}, //
                        {{1}, leaf}});
  Tree YZT = mtkv(false, {{{0, 0}, T},   //
                          {{0, 1}, T},   //
                          {{1, 0}, T},   //
                          {{1, 1}, T}}); //
  Tree XYZT = mtkv(false, {{{0}, YZT},   //
                           {{1}, YZT}});

  BOOST_TEST(*XYZT == *(flatten->output_tree));
  vector<std::string> exp_out_names{"X", "YZ", "T"};
  BOOST_CHECK_EQUAL_COLLECTIONS(flatten->output_levelnames.begin(), //
                                flatten->output_levelnames.end(),   //
                                exp_out_names.begin(),              //
                                exp_out_names.end());
}

BOOST_AUTO_TEST_CASE(test_level_remap_constructor) {
  TreeFactory<bool> f;
  TransformNetwork n;
  auto idr = transform_requests::Id({2, 4, 2}, {"X", "Y", "Z"});
  auto id = idr.join(f, 0, n);

  auto level_remap_r = transform_requests::LevelRemap("Y", {2, 3, 0, 1});
  auto level_remap = level_remap_r.join(f, id, n);

  Tree leaf = mtkv(true, {});
  Tree Z = mtkv(false, {{{0}, leaf}, //
                        {{1}, leaf}});
  Tree YZ = mtkv(false, {{{2}, Z},   //
                         {{3}, Z},   //
                         {{0}, Z},   //
                         {{1}, Z}}); //
  Tree XYZ = mtkv(false, {{{0}, YZ}, //
                          {{1}, YZ}});

  BOOST_TEST(*XYZ == *(level_remap->output_tree));
  vector<std::string> exp_out_names{"X", "Y", "Z"};
  BOOST_CHECK_EQUAL_COLLECTIONS(level_remap->output_levelnames.begin(), //
                                level_remap->output_levelnames.end(),   //
                                exp_out_names.begin(),                  //
                                exp_out_names.end());
}

BOOST_AUTO_TEST_CASE(test_level_swap_constructor) {
  TreeFactory<bool> f;
  TransformNetwork n;
  auto idr = transform_requests::Id({3, 2, 1}, {"X", "Y", "Z"});
  auto id = idr.join(f, 0, n);

  auto level_swap_r = transform_requests::LevelSwap1({"Z", "Y", "X"});
  auto level_swap = level_swap_r.join(f, id, n);

  Tree leaf = mtkv(true, {});
  Tree X = mtkv(false, {{{0}, leaf}, //
                        {{1}, leaf}, //
                        {{2}, leaf}});
  Tree YX = mtkv(false, {{{0}, X},   //
                         {{1}, X}}); //
  Tree ZYX = mtkv(false, {{{0}, YX}});

  BOOST_TEST(*ZYX == *(level_swap->output_tree));
  vector<std::string> exp_out_names{"Z", "Y", "X"};
  BOOST_CHECK_EQUAL_COLLECTIONS(level_swap->output_levelnames.begin(), //
                                level_swap->output_levelnames.end(),   //
                                exp_out_names.begin(),                 //
                                exp_out_names.end());
}

BOOST_AUTO_TEST_CASE(test_ro_naive_constructor) {
  TreeFactory<bool> f;
  TransformNetwork n;
  auto idr = transform_requests::Id({3, 2, 2}, {"X", "Y", "Z"});
  transformers::TransformerP id = idr.join(f, 0, n);

  auto flattenr = transform_requests::Flatten("Y", "Z", "YZ");
  transformers::TransformerP flatten = flattenr.join(f, id, n);

  auto eor = transform_requests::EONaive("YZ", "EO");
  transformers::TransformerP eo = eor.join(f, flatten, n);

  Tree leaf = mtkv(true, {});
  Tree YZE = mtkv(false, {{{0}, leaf}, //
                          {{3}, leaf}});
  Tree YZO = mtkv(false, {{{1}, leaf}, //
                          {{2}, leaf}});
  Tree EOYZ = mtkv(false, {{{}, YZE}, //
                           {{}, YZO}});
  Tree XEOYZ = mtkv(false, {{{0}, EOYZ}, //
                            {{1}, EOYZ}, //
                            {{2}, EOYZ}});
  BOOST_TEST(*XEOYZ == *(eo->output_tree));
  vector<std::string> exp_out_names{"X", "EO", "YZ"};
  BOOST_CHECK_EQUAL_COLLECTIONS(eo->output_levelnames.begin(), //
                                eo->output_levelnames.end(),   //
                                exp_out_names.begin(),         //
                                exp_out_names.end());
}

BOOST_AUTO_TEST_CASE(test_sum_constructor) {
  TreeFactory<bool> f;
  TransformNetwork n;
  auto idr = transform_requests::Id({3, 3}, {"X", "Y"}, "ROOT");
  transformers::TransformerP id = idr.join(f, 0, n);
  n.add_node(id, idr.get_end_node_name());

  using transform_requests::LevelRemap;
  auto level_remap_r1 = std::make_shared<LevelRemap>(std::string("Y"), //
                                                     vector<int>{2, 0, 1});
  auto level_remap_r2 = std::make_shared<LevelRemap>(std::string("Y"), //
                                                     vector<int>{2});
  auto sumr = transform_requests::Sum("SUM",           //
                                      "SUM",           //
                                      {level_remap_r1, //
                                       level_remap_r2});
  transformers::TransformerP sum = sumr.join(f, id, n);

  BOOST_TEST(n.nnodes() == 3);

  Tree leaf = mtkv(true, {});
  Tree Yrmp1 = mtkv(false, {{{2}, leaf}, //
                            {{0}, leaf}, //
                            {{1}, leaf}});
  Tree Yrmp2 = mtkv(false, {{{2}, leaf}});
  Tree XYrmp1 = mtkv(false, {{{0}, Yrmp1},   //
                             {{1}, Yrmp1},   //
                             {{2}, Yrmp1}}); //
  Tree XYrmp2 = mtkv(false, {{{0}, Yrmp2},   //
                             {{1}, Yrmp2},   //
                             {{2}, Yrmp2}}); //

  Tree SumXY = mtkv(false, {{{}, XYrmp1}, //
                            {{}, XYrmp2}});
  BOOST_TEST(*SumXY == *(sum->output_tree));

  vector<std::string> exp_out_names{"SUM", "X", "Y"};
  BOOST_CHECK_EQUAL_COLLECTIONS(sum->output_levelnames.begin(), //
                                sum->output_levelnames.end(),   //
                                exp_out_names.begin(),          //
                                exp_out_names.end());
}

BOOST_AUTO_TEST_CASE(test_fork_constructor) {
  TreeFactory<bool> f;
  TransformNetwork n;
  auto idr = transform_requests::Id({3, 3}, {"X", "Y"});
  transformers::TransformerP id = idr.join(f, 0, n);
  n.add_node(id, idr.get_end_node_name());

  using transform_requests::LevelRemap;
  auto level_remap_r1 = std::make_shared<LevelRemap>(std::string("Y"), //
                                                     vector<int>{2, 0, 1});

  auto level_remap_r2 = std::make_shared<LevelRemap>(std::string("Y"), //
                                                     vector<int>{2});
  auto forkr = transform_requests::Fork({level_remap_r1, //
                                         level_remap_r2});
  transformers::TransformerP fork = forkr.join(f, id, n);

  BOOST_TEST(n.nnodes() == 3);
  BOOST_TEST(fork == transformers::TransformerP(0));
}

BOOST_AUTO_TEST_CASE(test_composition_constructor) {
  TreeFactory<bool> f;
  TransformNetwork n;
  auto idr = transform_requests::Id({4, 8}, {"X", "Y"});
  transformers::TransformerP id = idr.join(f, 0, n);
  n.add_node(id, idr.get_end_node_name());
  using transform_requests::BB;
  using transform_requests::Q;
  using transform_requests::TreeComposition;
  auto qr = std::make_shared<Q>(std::string("Y"),     //
                                2,                    //
                                std::string("MPI Y"), //
                                "QY");
  auto bbr = std::make_shared<BB>(std::string("Y"),    //
                                  1,                   //
                                  std::string("BB Y"), //
                                  "BBY");
  TreeComposition compositionr({qr, bbr});
  auto composition = compositionr.join(f, id, n);
  // Last node in the chain is the output node,
  // and is not added to the tree:
  // this is caller responsibility.
  BOOST_TEST(n.nnodes() == 2);
  n.add_node(composition, "BBY");
  BOOST_TEST(n["BBY"]->output_tree == composition->output_tree);
}

BOOST_AUTO_TEST_CASE(test_build) {
  TreeFactory<bool> f;
  TransformNetwork n;
  using transform_requests::BB;
  using transform_requests::Id;
  using transform_requests::Q;
  auto idr = std::make_shared<Id>(vector<int>{4, 8}, //
                                  vector<std::string>{"X", "Y"});
  auto qr = std::make_shared<Q>(std::string("Y"),     //
                                2,                    //
                                std::string("MPI Y"), //
                                "QY");
  auto bbr = std::make_shared<BB>(std::string("Y"),    //
                                  1,                   //
                                  std::string("BB Y"), //
                                  "BBY");
  transform_requests::Build(f, //
                            n, //
                            {idr, qr, bbr});
  BOOST_TEST(n.nnodes() == 3);
}

BOOST_AUTO_TEST_SUITE_END()
