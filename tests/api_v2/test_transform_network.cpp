#include "api_v2/transform_network.hpp"
#include "api_v2/transform_request_makers.hpp"
#include "geometry/geometry.hpp"
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <memory>
#include <stdexcept>

using namespace hypercubes::slow::internals;

BOOST_AUTO_TEST_SUITE(test_transform_network)
using hypercubes::slow::BoundaryCondition;
using transform_networks::TransformNetwork;
using transform_requests::Build;
using namespace trms;

BOOST_AUTO_TEST_CASE(test_network_build_small) {
  TreeFactory f;
  TransformNetwork n;
  Build(f, n,
        {trms::Id({4, 4}, {"X", "Y"}, "ROOT"),
         trms::TreeComposition(
             {trms::QFull("X", 2, "MPI X", 0, BoundaryCondition::OPEN), //
              trms::QFull("Y", 2, "MPI Y", 0, BoundaryCondition::OPEN,
                          "MPI_DOMAIN_DECOMPOSITION")})});
  BOOST_TEST(n.nnodes() == 3);
  auto levelnames = n["MPI_DOMAIN_DECOMPOSITION"]->output_levelnames;
  vector<std::string> exp_levelnames{"MPI X", "X", "MPI Y", "Y"};

  // check names
  BOOST_CHECK_EQUAL_COLLECTIONS(levelnames.begin(), levelnames.end(), //
                                exp_levelnames.begin(), exp_levelnames.end());

  // check output tree
  auto leaf = mtkv(LEAF, {});
  // Since Y was Q-ized last, it is not yet renumbered.
  auto Y0 = mtkv(NODE, {{{0}, leaf}, //
                        {{1}, leaf}});
  auto Y1 = mtkv(NODE, {{{2}, leaf}, //
                        {{3}, leaf}});

  auto MPI_YY = mtkv(NODE, {{{}, Y0}, //
                            {{}, Y1}});
  auto XMPI_YY = mtkv(NODE, {{{0}, MPI_YY}, //
                             {{1}, MPI_YY}});
  auto MPI_XXMPI_YY = mtkv(NODE, {{{0}, XMPI_YY}, //
                                  {{1}, XMPI_YY}});
  BOOST_TEST(*(n["MPI_DOMAIN_DECOMPOSITION"]->output_tree) == *MPI_XXMPI_YY);
}

struct BuildFork1 {
  TreeFactory f;
  TransformNetwork n;
  BuildFork1() {
    Build(f, n,
          {
              Id({12, 12},   //
                 {"X", "Y"}, //
                 "root"),    //
              TreeComposition(
                  {QFull("X", 2, "MPI X", 1, BoundaryCondition::OPEN), //
                   QFull("Y", 2, "MPI Y", 1, BoundaryCondition::OPEN), //
                   Renumber(),
                   LevelSwap({"MPI X", "MPI Y", //
                              "X", "Y"})},
                  "domain decomposition"),

              Fork({TreeComposition({HBB("X", 1, "BB X", "bbx"), //
                                     HBB("Y", 1, "BB Y", "bby"), //
                                     Renumber(),                 //
                                     LevelSwap({"BB X", "X",     //
                                                "BB Y", "Y"},    //
                                               {"BB X", "BB Y",  //
                                                "X", "Y"},
                                               "vector level swap")},
                                    "mpi-border-bulk"),
                    TreeComposition({QSub("X", 2, "Vec X", 1, 1, "vecX"), //
                                     QSub("Y", 2, "Vec Y", 1, 1, "vecY"), //
                                     Renumber(),                          //
                                     LevelSwap({"Vec X", "X",             //
                                                "Vec Y", "Y"},            //
                                               {"X", "Y",                 //
                                                "Vec X", "Vec Y"})},
                                    "vector")}) //
          });
  }
};

struct BuildFork4 {
  TreeFactory f;
  TransformNetwork n;
  BuildFork4() {
    Build(f, n,
          {
              Id({42, 42, 42, 42},     //
                 {"X", "Y", "Z", "T"}, //
                 "root"),              //
              TreeComposition(
                  {QFull("X", 4, "MPI X", 1, BoundaryCondition::OPEN), //
                   QFull("Y", 4, "MPI Y", 1, BoundaryCondition::OPEN), //
                   QFull("Z", 4, "MPI Z", 1, BoundaryCondition::OPEN), //
                   QFull("T", 4, "MPI T", 1, BoundaryCondition::OPEN), //
                   Renumber(),
                   LevelSwap({"MPI X", "MPI Y", //
                              "MPI Z", "MPI T", //
                              "X", "Y",         //
                              "Z", "T"})},
                  "domain decomposition"),
              Fork({TreeComposition({HBB("X", 1, "BB X", "BB X"), //
                                     HBB("Y", 1, "BB Y", "BB Y"), //
                                     HBB("Z", 1, "BB Z", "BB Z"), //
                                     HBB("T", 1, "BB T", "BB T"), //
                                     Renumber(),                  //
                                     LevelSwap({"BB X", "X",      //
                                                "BB Y", "Y",      //
                                                "BB Z", "Z",      //
                                                "BB T", "T"},     //
                                               {"BB X", "BB Y",   //
                                                "BB Z", "BB T",   //
                                                "X", "Y",         //
                                                "Z", "T"}),
                                     Flatten("X", "T", "XYZT"),
                                     EONaive("XYZT", "EO")},
                                    "mpi-border-bulk"),
                    TreeComposition({QSub("X", 2, "Vec X", 1, 1),   //
                                     QSub("Y", 2, "Vec Y", 1, 1),   //
                                     QSub("Z", 2, "Vec Z", 1, 1),   //
                                     QSub("T", 2, "Vec T", 1, 1),   //
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
  TreeFactory f;
  TransformNetwork n;
  auto R1 = transform_requests::Id({4}, {"X"}, "root").join(f, 0, n);

  TransformNetwork::Arc a{R1, TransformNetwork::ArcType::DIRECT};
  TransformNetwork::Arc b{R1, TransformNetwork::ArcType::INVERSE};

  bool success = a < b;

  BOOST_TEST(success);
}

BOOST_AUTO_TEST_CASE(test_arc_ordering_different_t_same_direction) {
  TreeFactory f;
  TransformNetwork n;
  auto R1 = transform_requests::Id({4}, {"X"}, "root").join(f, 0, n);
  auto R2 = transform_requests::Id({5}, {"X"}, "root").join(f, 0, n);

  TransformNetwork::Arc a{R1, TransformNetwork::ArcType::DIRECT};
  TransformNetwork::Arc b{R2, TransformNetwork::ArcType::DIRECT};

  bool success = a < b or b < a;
  BOOST_TEST(success);
}

BOOST_AUTO_TEST_CASE(test_network_build_nodenames) {
  TreeFactory f;
  TransformNetwork n;
  Build(f, n,
        {Id({4, 4}, {"X", "Y"}, "ROOT"),
         TreeComposition(
             {QFull("X", 2, "MPI X", 0, BoundaryCondition::OPEN, "XMPI"),  //
              QFull("Y", 2, "MPI Y", 0, BoundaryCondition::OPEN, "YMPI")}, //
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
  TreeFactory f;
  TransformNetwork n;
  BOOST_CHECK_THROW(
      Build(f, n,
            {Id({4, 4}, {"X", "Y"}, "ROOT"),
             QFull("X", 2, "MPI X", 0, BoundaryCondition::OPEN, "ROOT")}),
      std::invalid_argument);
  f.print_diagnostics();
}
// TODO: convert this into a test for LevelSwap,
//       which is obviously fucked up
BOOST_AUTO_TEST_CASE(test_network_Q_swap) {
  TreeFactory f;
  TransformNetwork n;
  Build(f, n,
        {
            Id({4, 4},     //
               {"X", "Y"}, //
               "root"),    //
            TreeComposition(
                {QFull("X", 2, "MPI X", 0, BoundaryCondition::OPEN), //
                 QFull("Y", 2, "MPI Y", 0, BoundaryCondition::OPEN), //
                 Renumber(),
                 LevelSwap({"MPI X", "MPI Y", //
                            "X", "Y"})},
                "domain decomposition"),
        });

  BOOST_TEST(n.nnodes() == 5);
  BOOST_TEST(n.narcs() == 10);
  auto leaf = mtkv(LEAF, {});
  auto Y = mtkv(NODE, {{{0}, leaf}, //
                       {{1}, leaf}});
  auto XY = mtkv(NODE, {{{0}, Y}, //
                        {{1}, Y}});
  auto MPIY_XY = mtkv(NODE, {{{0}, XY}, //
                             {{1}, XY}});
  auto MPIX_MPIY_XY = mtkv(NODE, {{{0}, MPIY_XY}, //
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
  BOOST_TEST(n.find_transformations("root", "domain decomposition").size() ==
             4);
  BOOST_TEST(n.find_transformations("domain decomposition", "root").size() ==
             4);
  BOOST_TEST(n.find_transformations("root", "vector").size() == 8);
  BOOST_TEST(n.find_transformations("root", "mpi-border-bulk").size() == 8);
  BOOST_TEST(n.find_transformations("vector", "domain decomposition").size() ==
             4);
  BOOST_TEST(n.find_transformations("vector", "bbx").size() == 5);
  BOOST_TEST(n.find_transformations("vector", "bby").size() == 6);
}

BOOST_AUTO_TEST_CASE(test_get_transformer_from_arc_direct) {
  TreeFactory f;

  using transformers::Id;
  using transformers::QFull;

  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 4, 4},
                                vector<std::string>{"X", "Y", "Z"});

  auto q = std::make_shared<QFull>(f, R, "Y", //
                                   2,         //
                                   "MPI Y",   //
                                   0,         //
                                   BoundaryCondition::OPEN);

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
  TreeFactory f;

  using transformers::Id;
  using transformers::QFull;

  auto R = std::make_shared<Id>(f, //
                                vector<int>{4, 4, 4},
                                vector<std::string>{"X", "Y", "Z"});

  auto q = std::make_shared<QFull>(f, R, "Y", //
                                   2,         //
                                   "MPI Y",   //
                                   0,         //
                                   BoundaryCondition::OPEN);

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

BOOST_FIXTURE_TEST_CASE(test_get_transform, BuildFork1) {

  auto transform = n.get_transform("root", "mpi-border-bulk");
  vector<int> in{6, 1};
  vector<int> exp_out{1, 0, 0, 1, 0, 0};
  auto out = transform->apply(in)[0];
  BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), out.end(), //
                                exp_out.begin(), exp_out.end());
}

BOOST_AUTO_TEST_SUITE_END()
