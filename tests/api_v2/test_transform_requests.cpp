#include "api_v2/transform_requests.hpp"
#include "api_v2/transformer.hpp"
#include "trees/kvtree_data_structure.hpp"
#include "utils/print_utils.hpp"
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>

using namespace hypercubes::slow::internals;

BOOST_AUTO_TEST_SUITE(test_transform_requests)
using Tree = KVTreePv2<bool>;
BOOST_AUTO_TEST_CASE(test_id_constructor) {
  TreeFactory<bool> f;
  auto idr = transform_requests::Id({2, 2}, {"X", "Y"});
  auto id = idr.join(f, 0);

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
  auto idr = transform_requests::Id({2, 2}, {"X", "Y"});
  auto id = idr.join(f, 0);

  BOOST_CHECK_THROW(idr.join(f, id), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_q_constructor) {
  TreeFactory<bool> f;
  auto idr = transform_requests::Id({2, 4}, {"X", "Y"});
  auto id = idr.join(f, 0);

  auto qr = transform_requests::Q("Y", 2, "MPI Y");
  auto q = qr.join(f, id);
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
  auto idr = transform_requests::Id({2, 4}, {"X", "Y"});
  auto id = idr.join(f, 0);

  auto bbr = transform_requests::BB("Y", 1, "BB Y");
  auto bb = bbr.join(f, id);
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
  auto idr = transform_requests::Id({2, 2, 2, 2}, {"X", "Y", "Z", "T"});
  auto id = idr.join(f, 0);

  auto flattenr = transform_requests::Flatten("Y", "Z", "YZ");
  auto flatten = flattenr.join(f, id);

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
  auto idr = transform_requests::Id({2, 4, 2}, {"X", "Y", "Z"});
  auto id = idr.join(f, 0);

  auto level_remap_r = transform_requests::LevelRemap("Y", {2, 3, 0, 1});
  auto level_remap = level_remap_r.join(f, id);

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
  auto idr = transform_requests::Id({3, 2, 1}, {"X", "Y", "Z"});
  auto id = idr.join(f, 0);

  auto level_swap_r = transform_requests::LevelSwap({"Z", "Y", "X"});
  auto level_swap = level_swap_r.join(f, id);

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

BOOST_AUTO_TEST_SUITE_END()
