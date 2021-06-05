#include <boost/test/unit_test.hpp>

#include "api/memory_layout.hpp"
#include "partitioners/partitioners.hpp"

using namespace hypercubes::slow;
namespace pm = hypercubes::slow::partitioner_makers;

struct GridLike {

  enum { X, Y, Z, T, LOCAL, EXTRA };
  Sizes sizes;
  PartList partitioners;
  PartitionTree partition_tree;

  GridLike()
      : sizes({42, 42, 42, 42, 9}),
        partitioners({
            pm::QPeriodic("MPI X", X, 4),                  //
            pm::QPeriodic("MPI Y", Y, 4),                  //
            pm::QPeriodic("MPI Z", Z, 4),                  //
            pm::QPeriodic("MPI T", T, 4),                  //
            pm::QOpen("Vector X", X, 2),                   //
            pm::QOpen("Vector Y", Y, 2),                   //
            pm::HBB("Halo X", X, 1),                       //
            pm::HBB("Halo Y", Y, 1),                       //
            pm::HBB("Halo Z", Z, 1),                       //
            pm::HBB("Halo T", T, 1),                       //
            pm::EO("EO", {true, true, true, true, false}), //
            pm::Plain("Local", LOCAL),                     //
            pm::Plain("Extra", EXTRA),                     //
            pm::Site()                                     //
        }),
        partition_tree(sizes, partitioners) {}
};

BOOST_AUTO_TEST_SUITE(test_grid_like)

BOOST_FIXTURE_TEST_CASE(test_grid_like_nn, GridLike) {}

BOOST_AUTO_TEST_SUITE_END()
