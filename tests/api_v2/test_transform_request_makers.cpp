#include "api_v2/transform_network.hpp"
#include "api_v2/transform_request_makers.hpp"
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>

using namespace hypercubes::slow::internals;

BOOST_AUTO_TEST_SUITE(test_transform_request_makers)
using transform_networks::TransformNetwork;
using transform_requests::Build;
BOOST_AUTO_TEST_CASE(test_network_build_small) {
  TreeFactory<bool> f;
  TransformNetwork n;
  Build(f, n,
        {trms::Id({4, 4}, {"X", "Y"}, "ROOT"),
         trms::TreeComposition(
             {trms::Q("X", 2, "MPI X"), //
              trms::Q("Y", 2, "MPI Y", "MPI_DOMAIN_DECOMPOSITION")})});
  BOOST_TEST(n.nnodes() == 3);
  auto levelnames = n["MPI_DOMAIN_DECOMPOSITION"]->output_levelnames;
  vector<std::string> exp_levelnames{"MPI X", "X", "MPI Y", "Y"};

  // check names
  BOOST_CHECK_EQUAL_COLLECTIONS(levelnames.begin(), levelnames.end(), //
                                exp_levelnames.begin(), exp_levelnames.end());

  // check output tree
  auto leaf = mtkv(true, {});
  // Since Y was Q-ized last, it is not yet renumbered.
  auto Y0 = mtkv(false, {{{0}, leaf}, //
                         {{1}, leaf}});
  auto Y1 = mtkv(false, {{{2}, leaf}, //
                         {{3}, leaf}});

  auto MPI_YY = mtkv(false, {{{}, Y0}, //
                             {{}, Y1}});
  auto XMPI_YY = mtkv(false, {{{0}, MPI_YY}, //
                              {{1}, MPI_YY}});
  auto MPI_XXMPI_YY = mtkv(false, {{{0}, XMPI_YY}, //
                                   {{1}, XMPI_YY}});
  BOOST_TEST(*(n["MPI_DOMAIN_DECOMPOSITION"]->output_tree) == *MPI_XXMPI_YY);
}

BOOST_AUTO_TEST_SUITE_END()
