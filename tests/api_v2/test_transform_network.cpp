#include "api_v2/transform_network.hpp"
#include "api_v2/transform_request_makers.hpp"
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <stdexcept>

using namespace hypercubes::slow::internals;

BOOST_AUTO_TEST_SUITE(test_transform_request_makers)
using transform_networks::TransformNetwork;
using transform_requests::Build;
using namespace trms;
BOOST_AUTO_TEST_CASE(test_network_build_nodenames) {
  TreeFactory<bool> f;
  TransformNetwork n;
  Build(f, n,
        {Id({4, 4}, {"X", "Y"}, "ROOT"),
         TreeComposition({Q("X", 2, "MPI X", "XMPI"),  //
                          Q("Y", 2, "MPI Y", "YMPI")}, //
                         "MPI_DOMAIN_DECOMPOSITION")});
  BOOST_TEST(n.nnodes() == 3);
  std::set<std::string> expnames{"ROOT", //
                                 "XMPI", //
                                 "YMPI", //
                                 "MPI_DOMAIN_DECOMPOSITION"};
  auto names = n.nodenames();
  BOOST_CHECK_EQUAL_COLLECTIONS(expnames.begin(), expnames.end(), //
                                names.begin(), names.end());
}

BOOST_AUTO_TEST_CASE(test_network_build_repeated_names_throws) {
  TreeFactory<bool> f;
  TransformNetwork n;
  BOOST_CHECK_THROW(
      Build(f, n, {Id({4, 4}, {"X", "Y"}, "ROOT"), Q("X", 2, "MPI X", "ROOT")}),
      std::invalid_argument);
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
}

BOOST_AUTO_TEST_CASE(test_network_build_fork) {
  TreeFactory<bool> f;
  TransformNetwork n;
  Build(f, n,
        {
            Id({8, 8},                           //
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

  BOOST_TEST(n.nnodes() == 13);
  f.print_diagnostics();
}

BOOST_AUTO_TEST_CASE(test_network_build_fork_4D) {
  // Ok but SLOW! ~12s
  TreeFactory<bool> f;
  TransformNetwork n;
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
                                              "Z", "T"},
                                             "vector level swap")},
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
                                             {"X", "Y", "Z", "T", //
                                              "Vec X", "Vec Y",   //
                                              "Vec Z", "Vec T"})},
                                  "vector")}) //
        });

  BOOST_TEST(n.nnodes() == 19);
  f.print_diagnostics();
}

BOOST_AUTO_TEST_SUITE_END()