#include "api_v2/transform_network.hpp"
#include "api_v2/transform_request_makers.hpp"
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <memory>
#include <stdexcept>

using namespace hypercubes::slow::internals;

BOOST_AUTO_TEST_SUITE(test_transform_request_makers)
using transform_networks::TransformNetwork;
using transform_requests::Build;
using namespace trms;

struct BuildFork1 {
  TreeFactory<bool> f;
  TransformNetwork n;
  BuildFork1() {
    Build(f, n,
          {
              Id({12, 12},                         //
                 {"X", "Y"},                       //
                 "root"),                          //
              TreeComposition({Q("X", 2, "MPI X"), //
                               Q("Y", 2, "MPI Y"), //
                               Renumber(),
                               LevelSwap({"MPI X", "MPI Y", //
                                          "X", "Y"})},
                              "domain decomposition"),

              Fork({TreeComposition({BB("X", 1, "BB X", "bbx"), //
                                     BB("Y", 1, "BB Y", "bby"), //
                                     Renumber(),                //
                                     LevelSwap({"BB X", "X",    //
                                                "BB Y", "Y"},   //
                                               {"BB X", "BB Y", //
                                                "X", "Y"},
                                               "vector level swap")},
                                    "mpi-border-bulk"),
                    TreeComposition({Q("X", 2, "Vec X", "vecX"), //
                                     Q("Y", 2, "Vec Y", "vecY"), //
                                     Renumber(),                 //
                                     LevelSwap({"Vec X", "X",    //
                                                "Vec Y", "Y"},   //
                                               {"X", "Y",        //
                                                "Vec X", "Vec Y"})},
                                    "vector")}) //
          });
  }
};

struct BuildFork4 {
  TreeFactory<bool> f;
  TransformNetwork n;
  BuildFork4() {
    Build(f, n,
          {
              Id({42, 42, 42, 42},                 //
                 {"X", "Y", "Z", "T"},             //
                 "root"),                          //
              TreeComposition({Q("X", 4, "MPI X"), //
                               Q("Y", 4, "MPI Y"), //
                               Q("Z", 4, "MPI Z"), //
                               Q("T", 4, "MPI T"), //
                               Renumber(),
                               LevelSwap({"MPI X", "MPI Y", //
                                          "MPI Z", "MPI T", //
                                          "X", "Y",         //
                                          "Z", "T"})},
                              "domain decomposition"),
              Fork({TreeComposition({BB("X", 1, "BB X", "BB X"), //
                                     BB("Y", 1, "BB Y", "BB Y"), //
                                     BB("Z", 1, "BB Z", "BB Z"), //
                                     BB("T", 1, "BB T", "BB T"), //
                                     Renumber(),                 //
                                     LevelSwap({"BB X", "X",     //
                                                "BB Y", "Y",     //
                                                "BB Z", "Z",     //
                                                "BB T", "T"},    //
                                               {"BB X", "BB Y",  //
                                                "BB Z", "BB T",  //
                                                "X", "Y",        //
                                                "Z", "T"}),
                                     Flatten("X", "T", "XYZT"),
                                     EONaive("XYZT", "EO")},
                                    "mpi-border-bulk"),
                    TreeComposition({Q("X", 2, "Vec X"),            //
                                     Q("Y", 2, "Vec Y"),            //
                                     Q("Z", 2, "Vec Z"),            //
                                     Q("T", 2, "Vec T"),            //
                                     Renumber(),                    //
                                     LevelSwap({"Vec X", "X",       //
                                                "Vec Y", "Y",       //
                                                "Vec Z", "Z",       //
                                                "Vec T", "T"},      //
                                               {"X", "Y",           //
                                                "Z", "T",           //
                                                "Vec X", "Vec Y",   //
                                                "Vec Z", "Vec T"}), //
                                     Flatten("X", "T", "XYZT"),     //
                                     EONaive("XYZT", "EO")},
                                    "vector")}) //
          });
  }
};

BOOST_AUTO_TEST_CASE(test_arc_ordering_same_t) {
  TreeFactory<bool> f;
  TransformNetwork n;
  auto R1 = transform_requests::Id({4}, {"X"}, "root").join(f, 0, n);

  TransformNetwork::Arc a{R1, TransformNetwork::ArcType::DIRECT};
  TransformNetwork::Arc b{R1, TransformNetwork::ArcType::INVERSE};

  bool success = a < b;

  BOOST_TEST(success);
}

BOOST_AUTO_TEST_CASE(test_arc_ordering_different_t_same_direction) {
  TreeFactory<bool> f;
  TransformNetwork n;
  auto R1 = transform_requests::Id({4}, {"X"}, "root").join(f, 0, n);
  auto R2 = transform_requests::Id({5}, {"X"}, "root").join(f, 0, n);

  TransformNetwork::Arc a{R1, TransformNetwork::ArcType::DIRECT};
  TransformNetwork::Arc b{R2, TransformNetwork::ArcType::DIRECT};

  bool success = a < b or b < a;
  BOOST_TEST(success);
}

BOOST_AUTO_TEST_CASE(test_network_build_nodenames) {
  TreeFactory<bool> f;
  TransformNetwork n;
  Build(f, n,
        {Id({4, 4}, {"X", "Y"}, "ROOT"),
         TreeComposition({Q("X", 2, "MPI X", "XMPI"),  //
                          Q("Y", 2, "MPI Y", "YMPI")}, //
                         "MPI_DOMAIN_DECOMPOSITION")});
  BOOST_TEST(n.nnodes() == 3);
  BOOST_TEST(n.narcs() == 2 * n.nnodes());
  std::set<std::string> expnames{"ROOT", //
                                 "XMPI", //
                                 "YMPI", //
                                 "MPI_DOMAIN_DECOMPOSITION"};
  auto names = n.nodenames();
  BOOST_CHECK_EQUAL_COLLECTIONS(expnames.begin(), expnames.end(), //
                                names.begin(), names.end());
  f.print_diagnostics();
}

BOOST_AUTO_TEST_CASE(test_network_build_repeated_names_throws) {
  TreeFactory<bool> f;
  TransformNetwork n;
  BOOST_CHECK_THROW(
      Build(f, n, {Id({4, 4}, {"X", "Y"}, "ROOT"), Q("X", 2, "MPI X", "ROOT")}),
      std::invalid_argument);
  f.print_diagnostics();
}
// TODO: convert this into a test for LevelSwap,
//       which is obviously fucked up
BOOST_AUTO_TEST_CASE(test_network_Q_swap) {
  TreeFactory<bool> f;
  TransformNetwork n;
  Build(f, n,
        {
            Id({4, 4},                           //
               {"X", "Y"},                       //
               "root"),                          //
            TreeComposition({Q("X", 2, "MPI X"), //
                             Q("Y", 2, "MPI Y"), //
                             Renumber(),
                             LevelSwap({"MPI X", "MPI Y", //
                                        "X", "Y"})},
                            "domain decomposition"),
        });

  BOOST_TEST(n.nnodes() == 5);
  BOOST_TEST(n.narcs() == 10);
  auto leaf = mtkv(true, {});
  auto Y = mtkv(false, {{{0}, leaf}, //
                        {{1}, leaf}});
  auto XY = mtkv(false, {{{0}, Y}, //
                         {{1}, Y}});
  auto MPIY_XY = mtkv(false, {{{0}, XY}, //
                              {{1}, XY}});
  auto MPIX_MPIY_XY = mtkv(false, {{{0}, MPIY_XY}, //
                                   {{1}, MPIY_XY}});

  BOOST_TEST(*(n["domain decomposition"]->output_tree) == *MPIX_MPIY_XY);
  f.print_diagnostics();
}

BOOST_FIXTURE_TEST_CASE(test_network_build_fork, BuildFork1) {

  BOOST_TEST(n.nnodes() == 13);
  BOOST_TEST(n.narcs() == 2 * n.nnodes());
  f.print_diagnostics();
}

BOOST_FIXTURE_TEST_CASE(test_network_build_fork_4D, BuildFork4) {
  BOOST_TEST(n.nnodes() == 23);
  BOOST_TEST(n.narcs() == 2 * n.nnodes());
  f.print_diagnostics();
}

BOOST_FIXTURE_TEST_CASE(test_network_find_transform_chain, BuildFork1) {
  BOOST_TEST(n.find_transform("root", "domain decomposition").size() == 4);
  BOOST_TEST(n.find_transform("domain decomposition", "root").size() == 4);
  BOOST_TEST(n.find_transform("root", "vector").size() == 8);
  BOOST_TEST(n.find_transform("root", "mpi-border-bulk").size() == 8);
  BOOST_TEST(n.find_transform("vector", "domain decomposition").size() == 4);
  BOOST_TEST(n.find_transform("vector", "bbx").size() == 5);
  BOOST_TEST(n.find_transform("vector", "bby").size() == 6);
}

BOOST_AUTO_TEST_CASE(test_get_transformer_from_arc_direct) {
  TreeFactory<bool> f;

  using transformers::Id;
  using transformers::Q;

  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 4, 4},
                                vector<std::string>{"X", "Y", "Z"});

  auto q = std::make_shared<Q>(f, R, "Y", 2, "MPI Y");

  TransformNetwork::Arc a{q, TransformNetwork::ArcType::DIRECT};

  auto q2 = TransformNetwork::get_transformer(a);

  // Check that q2 is the same as q1

  for (int I = 0; I < 4 * 4 * 4; ++I) {
    int is, js, ks;
    is = I % 4;
    js = (I / 4) % 4;
    ks = (I / 16);

    vector<int> in{is, js, ks};
    auto qout = q->apply(in);
    auto q2out = q2->apply(in);
    BOOST_CHECK_EQUAL_COLLECTIONS(qout.begin(), qout.end(), //
                                  q2out.begin(), q2out.end());
    auto should_be_in = q2->inverse(q->apply(in)[0])[0];
    BOOST_CHECK_EQUAL_COLLECTIONS(should_be_in.begin(), should_be_in.end(), //
                                  in.begin(), in.end());
  }
}
BOOST_AUTO_TEST_CASE(test_get_transformer_from_arc_inverse) {
  TreeFactory<bool> f;

  using transformers::Id;
  using transformers::Q;

  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 4, 4},
                                vector<std::string>{"X", "Y", "Z"});

  auto q = std::make_shared<Q>(f, R, "Y", 2, "MPI Y");

  TransformNetwork::Arc a{q, TransformNetwork::ArcType::INVERSE};

  auto q2 = TransformNetwork::get_transformer(a);

  // Check that q2 is the inverse of q1

  for (int I = 0; I < 4 * 4 * 4; ++I) {
    int is, js, ks;
    is = I % 4;
    js = (I / 4) % 4;
    ks = (I / 16);

    vector<int> in{is, js, ks};
    auto should_be_in = q2->apply(q->apply(in)[0])[0];
    BOOST_CHECK_EQUAL_COLLECTIONS(should_be_in.begin(), should_be_in.end(), //
                                  in.begin(), in.end());
  }
}

BOOST_AUTO_TEST_SUITE_END()
