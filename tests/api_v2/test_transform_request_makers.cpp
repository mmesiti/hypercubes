#include "api_v2/transform_network.hpp"
#include "api_v2/transform_request_makers.hpp"
#include <boost/test/unit_test.hpp>

using namespace hypercubes::slow::internals;

BOOST_AUTO_TEST_SUITE(test_transform_request_makers)
using transform_networks::TransformNetwork;
using transform_requests::Build;
BOOST_AUTO_TEST_CASE(test_id_maker) {
  TreeFactory<bool> f;
  TransformNetwork n;
  Build(f, n,
        {
            trms::Id({10, 10, 10, 10}, {"X", "Y", "Z", "T"}, "ROOT"),
            trms::TreeComposition({trms::Q("X", 2, "MPI X"), //
                                   trms::Q("Y", 2, "MPI Y"), //
                                   trms::Q("Z", 2, "MPI Z"), //
                                   trms::Q("T", 2, "MPI T")}),
            trms::LevelSwap({"MPI X", "MPI Y", "MPI Z", "MPI T", //
                             "X", "Y", "Z", "T"}),
            trms::TreeComposition({
                trms::TreeComposition(
                    {trms::BB("X", 1, "BB X"), //
                                               // trms::BB("Y", 1, "BB Y"), //
                     trms::BB("Z", 1, "BB Z"), //
                     trms::BB("T", 1, "BB T")}),
                // trms::LevelSwap({"MPI X", "MPI Y", "MPI Z", "MPI T", //
                //                 "BB X", "BB Y", "BB Z", "BB T",     //
                //                 "X", "Y", "Z", "T"}),
                // trms::Flatten("X", "T", "XYZT"),
                // trms::EONaive("XYZT", "EONaive", "DOMAIN_DECOMPOSITION_EO"),
            }),
        });
}

BOOST_AUTO_TEST_SUITE_END()
