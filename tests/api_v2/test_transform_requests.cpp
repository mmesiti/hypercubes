#include "api_v2/transform_network.hpp"
#include "api_v2/transform_requests.hpp"
#include "api_v2/transformer.hpp"
#include "geometry/geometry.hpp"
#include "trees/kvtree_data_structure.hpp"
#include "utils/print_utils.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <memory>

using namespace hypercubes::slow::internals;
using hypercubes::slow::BoundaryCondition;
BOOST_AUTO_TEST_SUITE(test_transform_requests)
using Tree = KVTreePv2<NodeType>;
using transform_networks::TransformNetwork;

BOOST_AUTO_TEST_CASE(test_id_constructor) {
  TreeFactory f;
  TransformNetwork n;
  auto idr = transform_requests::Id({2, 2}, {"X", "Y"});
  auto id = idr.join(f, 0, n);

  Tree leaf = mtkv(LEAF, {});
  Tree subtree = mtkv(NODE, {{{0}, leaf}, //
                             {{1}, leaf}});

  Tree exptree = mtkv(NODE, {{{0}, subtree}, //
                             {{1}, subtree}});
  BOOST_TEST(*exptree == *(id->output_tree));

  vector<std::string> exp_out_names{"X", "Y"};
  BOOST_CHECK_EQUAL_COLLECTIONS(id->output_levelnames.begin(), //
                                id->output_levelnames.end(),   //
                                exp_out_names.begin(),         //
                                exp_out_names.end());
}

BOOST_AUTO_TEST_CASE(test_id_join_throws) {
  TreeFactory f;
  TransformNetwork n;
  auto idr = transform_requests::Id({2, 2}, {"X", "Y"});
  auto id = idr.join(f, 0, n);

  BOOST_CHECK_THROW(idr.join(f, id, n), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_q_constructor) {
  TreeFactory f;
  TransformNetwork n;
  auto idr = transform_requests::Id({2, 4}, {"X", "Y"});
  auto id = idr.join(f, 0, n);

  auto qr =
      transform_requests::QFull("Y", 2, "MPI Y", 0, BoundaryCondition::OPEN);
  auto q = qr.join(f, id, n);
  Tree leaf = mtkv(LEAF, {});
  Tree subsubtree1 = mtkv(NODE, {{{0}, leaf}, //
                                 {{1}, leaf}});
  Tree subsubtree2 = mtkv(NODE, {{{2}, leaf}, //
                                 {{3}, leaf}});

  Tree subtree = mtkv(NODE, {{{}, subsubtree1}, //
                             {{}, subsubtree2}});
  Tree exptree = mtkv(NODE, {{{0}, subtree}, //
                             {{1}, subtree}});

  BOOST_TEST(*exptree == *(q->output_tree));
  vector<std::string> exp_out_names{"X", "MPI Y", "Y"};
  BOOST_CHECK_EQUAL_COLLECTIONS(q->output_levelnames.begin(), //
                                q->output_levelnames.end(),   //
                                exp_out_names.begin(),        //
                                exp_out_names.end());
}

BOOST_AUTO_TEST_CASE(test_hbb_constructor) {
  TreeFactory f;
  TransformNetwork n;
  auto idr = transform_requests::Id({2, 8}, {"X", "Y"});
  auto id = idr.join(f, 0, n);

  auto partr = transform_requests::QFull("Y",     //
                                         2,       //
                                         "MPI_Y", //
                                         1,       //
                                         BoundaryCondition::OPEN);
  auto part = partr.join(f, id, n);

  auto hbbr = transform_requests::HBB("Y", 1, "BB Y");
  auto hbb = hbbr.join(f, part, n);
  Tree leaf = mtkv(LEAF, {});

  Tree ytree = mtkv(NODE, {{{0},
                            mtkv(NODE, {{{},                         //
                                         mtkv(NODE, {{{0}, leaf}})}, //
                                        {{},                         //
                                         mtkv(NODE, {{{1}, leaf}})}, //
                                        {{},                         //
                                         mtkv(NODE, {{{2}, leaf},    //
                                                     {{3}, leaf}})}, //
                                        {{},                         //
                                         mtkv(NODE, {{{4}, leaf}})}, //
                                        {{},                         //
                                         mtkv(NODE, {{{5}, leaf}})}})},
                           {{1},
                            mtkv(NODE, {{{},                         //
                                         mtkv(NODE, {{{0}, leaf}})}, //
                                        {{},                         //
                                         mtkv(NODE, {{{1}, leaf}})}, //
                                        {{},                         //
                                         mtkv(NODE, {{{2}, leaf},    //
                                                     {{3}, leaf}})}, //
                                        {{},                         //
                                         mtkv(NODE, {{{4}, leaf}})}, //
                                        {{},                         //
                                         mtkv(NODE, {{{5}, leaf}})}})}});

  Tree exptree = mtkv(NODE, {{{0}, ytree}, //
                             {{1}, ytree}});

  BOOST_TEST(*exptree == *(hbb->output_tree));
  vector<std::string> exp_out_names{"X", "MPI_Y", "BB Y", "Y"};
  BOOST_CHECK_EQUAL_COLLECTIONS(hbb->output_levelnames.begin(), //
                                hbb->output_levelnames.end(),   //
                                exp_out_names.begin(),          //
                                exp_out_names.end());
}

BOOST_AUTO_TEST_CASE(test_flatten_constructor) {
  TreeFactory f;
  TransformNetwork n;
  auto idr = transform_requests::Id({2, 2, 2, 2}, {"X", "Y", "Z", "T"});
  auto id = idr.join(f, 0, n);

  auto flattenr = transform_requests::Flatten("Y", "Z", "YZ");
  auto flatten = flattenr.join(f, id, n);

  Tree leaf = mtkv(LEAF, {});
  Tree T = mtkv(NODE, {{{0}, leaf}, //
                       {{1}, leaf}});
  Tree YZT = mtkv(NODE, {{{0, 0}, T},   //
                         {{0, 1}, T},   //
                         {{1, 0}, T},   //
                         {{1, 1}, T}}); //
  Tree XYZT = mtkv(NODE, {{{0}, YZT},   //
                          {{1}, YZT}});

  BOOST_TEST(*XYZT == *(flatten->output_tree));
  vector<std::string> exp_out_names{"X", "YZ", "T"};
  BOOST_CHECK_EQUAL_COLLECTIONS(flatten->output_levelnames.begin(), //
                                flatten->output_levelnames.end(),   //
                                exp_out_names.begin(),              //
                                exp_out_names.end());
}
BOOST_AUTO_TEST_CASE(test_collect_leaves_constructor) {
  TreeFactory f;
  TransformNetwork n;
  auto idr = transform_requests::Id({2, 2, 3}, {"X", "Y", "Z"});
  auto id = idr.join(f, 0, n);

  auto collect_leaves_r = transform_requests::CollectLeaves("Y", "COLLECT", 8);
  auto collect_leaves = collect_leaves_r.join(f, id, n);

  Tree leaf = mtkv(LEAF, {});
  auto nkc = TreeFactory::no_key;
  Tree COLLECT = mtkv(NODE, {{{0, 0}, leaf}, //
                             {{0, 1}, leaf}, //
                             {{0, 2}, leaf}, //
                             {{1, 0}, leaf}, //
                             {{1, 1}, leaf}, //
                             {{1, 2}, leaf}, //
                             // these do not exist
                             // in the original tree
                             {{nkc, nkc}, leaf}, //
                             {{nkc, nkc}, leaf}});
  Tree X_COLLECT = mtkv(NODE, {{{0}, COLLECT}, //
                               {{1}, COLLECT}});

  BOOST_TEST(*X_COLLECT == *(collect_leaves->output_tree));
  vector<std::string> exp_out_names{"X", "COLLECT"};
  BOOST_CHECK_EQUAL_COLLECTIONS(exp_out_names.begin(), exp_out_names.end(),
                                collect_leaves->output_levelnames.begin(),
                                collect_leaves->output_levelnames.end());
}
BOOST_AUTO_TEST_CASE(test_level_remap_constructor) {
  TreeFactory f;
  TransformNetwork n;
  auto idr = transform_requests::Id({2, 4, 2}, {"X", "Y", "Z"});
  auto id = idr.join(f, 0, n);

  auto level_remap_r = transform_requests::LevelRemap("Y", {2, 3, 0, 1});
  auto level_remap = level_remap_r.join(f, id, n);

  Tree leaf = mtkv(LEAF, {});
  Tree Z = mtkv(NODE, {{{0}, leaf}, //
                       {{1}, leaf}});
  Tree YZ = mtkv(NODE, {{{2}, Z},   //
                        {{3}, Z},   //
                        {{0}, Z},   //
                        {{1}, Z}}); //
  Tree XYZ = mtkv(NODE, {{{0}, YZ}, //
                         {{1}, YZ}});

  BOOST_TEST(*XYZ == *(level_remap->output_tree));
  vector<std::string> exp_out_names{"X", "Y", "Z"};
  BOOST_CHECK_EQUAL_COLLECTIONS(level_remap->output_levelnames.begin(), //
                                level_remap->output_levelnames.end(),   //
                                exp_out_names.begin(),                  //
                                exp_out_names.end());
}

BOOST_AUTO_TEST_CASE(test_level_swap_constructor) {
  TreeFactory f;
  TransformNetwork n;
  auto idr = transform_requests::Id({3, 2, 1}, {"X", "Y", "Z"});
  auto id = idr.join(f, 0, n);

  auto level_swap_r = transform_requests::LevelSwap1({"Z", "Y", "X"});
  auto level_swap = level_swap_r.join(f, id, n);

  Tree leaf = mtkv(LEAF, {});
  Tree X = mtkv(NODE, {{{0}, leaf}, //
                       {{1}, leaf}, //
                       {{2}, leaf}});
  Tree YX = mtkv(NODE, {{{0}, X},   //
                        {{1}, X}}); //
  Tree ZYX = mtkv(NODE, {{{0}, YX}});

  BOOST_TEST(*ZYX == *(level_swap->output_tree));
  vector<std::string> exp_out_names{"Z", "Y", "X"};
  BOOST_CHECK_EQUAL_COLLECTIONS(level_swap->output_levelnames.begin(), //
                                level_swap->output_levelnames.end(),   //
                                exp_out_names.begin(),                 //
                                exp_out_names.end());
}

BOOST_AUTO_TEST_CASE(test_eo_naive_constructor) {
  TreeFactory f;
  TransformNetwork n;
  auto idr = transform_requests::Id({3, 2, 2}, {"X", "Y", "Z"});
  transformers::TransformerP id = idr.join(f, 0, n);

  auto flattenr = transform_requests::Flatten("Y", "Z", "YZ");
  transformers::TransformerP flatten = flattenr.join(f, id, n);

  auto eor = transform_requests::EONaive("YZ", "EO");
  transformers::TransformerP eo = eor.join(f, flatten, n);

  Tree leaf = mtkv(LEAF, {});
  Tree YZE = mtkv(NODE, {{{0}, leaf}, //
                         {{3}, leaf}});
  Tree YZO = mtkv(NODE, {{{1}, leaf}, //
                         {{2}, leaf}});
  Tree EOYZ = mtkv(NODE, {{{}, YZE}, //
                          {{}, YZO}});
  Tree XEOYZ = mtkv(NODE, {{{0}, EOYZ}, //
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
  TreeFactory f;
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
                                      {level_remap_r1, //
                                       level_remap_r2},
                                      "SUM");
  transformers::TransformerP sum = sumr.join(f, id, n);

  BOOST_TEST(n.nnodes() == 3);

  Tree leaf = mtkv(LEAF, {});
  Tree Yrmp1 = mtkv(NODE, {{{2}, leaf}, //
                           {{0}, leaf}, //
                           {{1}, leaf}});
  Tree Yrmp2 = mtkv(NODE, {{{2}, leaf}});
  Tree XYrmp1 = mtkv(NODE, {{{0}, Yrmp1},   //
                            {{1}, Yrmp1},   //
                            {{2}, Yrmp1}}); //
  Tree XYrmp2 = mtkv(NODE, {{{0}, Yrmp2},   //
                            {{1}, Yrmp2},   //
                            {{2}, Yrmp2}}); //

  Tree SumXY = mtkv(NODE, {{{}, XYrmp1}, //
                           {{}, XYrmp2}});
  BOOST_TEST(*SumXY == *(sum->output_tree));

  vector<std::string> exp_out_names{"SUM", "X", "Y"};
  BOOST_CHECK_EQUAL_COLLECTIONS(sum->output_levelnames.begin(), //
                                sum->output_levelnames.end(),   //
                                exp_out_names.begin(),          //
                                exp_out_names.end());
}

BOOST_AUTO_TEST_CASE(test_fork_constructor) {
  TreeFactory f;
  TransformNetwork n;
  auto idr = transform_requests::Id({3, 3}, {"X", "Y"}, "ROOT");
  transformers::TransformerP id = idr.join(f, 0, n);
  n.add_node(id, idr.get_end_node_name());

  using transform_requests::LevelRemap;
  auto level_remap_r1 = std::make_shared<LevelRemap>(std::string("Y"),     //
                                                     vector<int>{2, 0, 1}, //
                                                     "Yremap1");

  auto level_remap_r2 = std::make_shared<LevelRemap>(std::string("Y"), //
                                                     vector<int>{2},   //
                                                     "Yremap2");
  auto forkr = transform_requests::Fork({level_remap_r1, //
                                         level_remap_r2});
  transformers::TransformerP fork = forkr.join(f, id, n);

  BOOST_TEST(n.nnodes() == 3);
  // forkr.join is the null pointer (as there is no single 'end')
  BOOST_TEST(fork == transformers::TransformerP(0));
}

BOOST_AUTO_TEST_CASE(test_composition_constructor) {
  TreeFactory f;
  TransformNetwork n;
  auto idr = transform_requests::Id({4, 8}, {"X", "Y"});
  transformers::TransformerP id = idr.join(f, 0, n);
  n.add_node(id, idr.get_end_node_name());
  using transform_requests::HBB;
  using transform_requests::QFull;
  using transform_requests::TreeComposition;
  auto qr =
      std::make_shared<QFull>(std::string("Y"),                          //
                              2,                                         //
                              std::string("MPI Y"),                      //
                              1,                                         //
                              hypercubes::slow::BoundaryCondition::OPEN, //
                              "QY");
  auto bbr = std::make_shared<HBB>(std::string("Y"),    //
                                   1,                   //
                                   std::string("BB Y"), //
                                   "BBY");
  TreeComposition compositionr({qr, bbr}, "COMPOSITION");
  auto composition = compositionr.join(f, id, n);
  // Last node in the chain is the output node.
  // It is added to the network by compositionr.join
  // because it is the only one.
  n.add_node(composition, "COMPOSITION");
  BOOST_TEST(n.nnodes() == 3);
  BOOST_TEST(n["BBY"]->output_tree == composition->output_tree);
  BOOST_TEST(n["COMPOSITION"] == n["BBY"]);
}

BOOST_AUTO_TEST_CASE(test_build) {
  TreeFactory f;
  TransformNetwork n;
  using transform_requests::HBB;
  using transform_requests::Id;
  using transform_requests::QFull;
  auto idr = std::make_shared<Id>(vector<int>{4, 8}, //
                                  vector<std::string>{"X", "Y"});
  auto qr =
      std::make_shared<QFull>(std::string("Y"),     //
                              2,                    //
                              std::string("MPI Y"), //
                              1,                    //
                              hypercubes::slow::BoundaryCondition::OPEN, "QY");
  auto bbr = std::make_shared<HBB>(std::string("Y"),    //
                                   1,                   //
                                   std::string("BB Y"), //
                                   "BBY");
  transform_requests::Build(f, //
                            n, //
                            {idr, qr, bbr});
  BOOST_TEST(n.nnodes() == 3);
}

BOOST_AUTO_TEST_SUITE_END()
