
#include "api_v2/transform_network.hpp"
#include "api_v2/transform_request_makers.hpp"
#include "api_v2/transform_requests.hpp"

#include <boost/test/unit_test.hpp>
using namespace hypercubes::slow::internals::trms;
using namespace hypercubes::slow::internals;
BOOST_AUTO_TEST_SUITE(test_composite_transform_requests)
using Tree = KVTreePv2<bool>;
using transform_networks::TransformNetwork;

BOOST_AUTO_TEST_CASE(test_build) {
  // vector<TransformRequestP> requests{Id({4, 4, 4}, {"X", "Y", "Z"}),
  //};
}

BOOST_AUTO_TEST_SUITE_END()
