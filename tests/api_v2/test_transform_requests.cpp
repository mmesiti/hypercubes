#include "api_v2/transform_requests.hpp"
#include "api_v2/transformer.hpp"
#include "trees/kvtree_data_structure.hpp"
#include "utils/print_utils.hpp"
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>

using namespace hypercubes::slow::internals;

BOOST_AUTO_TEST_SUITE(test_transform_requests)
BOOST_AUTO_TEST_CASE(test_id_constructor) {
  TreeFactory<bool> f;
  auto idr = transform_requests::Id({2, 2}, {"X", "Y"});
  auto id = idr.join(f, 0);

  KVTreePv2<bool> leaf = mtkv(true, {});
  KVTreePv2<bool> subtree = mtkv(false, {{{0}, leaf}, //
                                         {{1}, leaf}});

  KVTreePv2<bool> exptree = mtkv(false, {{{0}, subtree}, //
                                         {{1}, subtree}});
  BOOST_TEST(*exptree == *(id->output_tree));

  vector<std::string> exp_out_names{"X", "Y"};
  BOOST_CHECK_EQUAL_COLLECTIONS(id->output_levelnames.begin(), //
                                id->output_levelnames.end(),   //
                                exp_out_names.begin(),         //
                                exp_out_names.end());
}

BOOST_AUTO_TEST_CASE(test_q_constructor) {
  TreeFactory<bool> f;
  auto idr = transform_requests::Id({2, 4}, {"X", "Y"});
  auto id = idr.join(f, 0);

  auto qr = transform_requests::Q("Y", 2, "MPI Y");
  auto q = qr.join(f, id);
  KVTreePv2<bool> leaf = mtkv(true, {});
  KVTreePv2<bool> subsubtree1 = mtkv(false, {{{0}, leaf}, //
                                             {{1}, leaf}});
  KVTreePv2<bool> subsubtree2 = mtkv(false, {{{2}, leaf}, //
                                             {{3}, leaf}});

  KVTreePv2<bool> subtree = mtkv(false, {{{}, subsubtree1}, //
                                         {{}, subsubtree2}});
  KVTreePv2<bool> exptree = mtkv(false, {{{0}, subtree}, //
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
  using Tree = KVTreePv2<bool>;
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

BOOST_AUTO_TEST_SUITE_END()
