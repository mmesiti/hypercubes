#include "geometry/geometry.hpp"
#include <boost/test/unit_test.hpp>

using namespace hypercubes::slow;

BOOST_AUTO_TEST_SUITE(test_geometry)

BOOST_AUTO_TEST_CASE(test_up) {
  Coordinates xs{1, 2, 3, 4};
  BOOST_TEST(up(xs, 2) == (Coordinates{1, 2, 4, 4}));
}

BOOST_AUTO_TEST_CASE(test_down) {
  Coordinates xs{1, 2, 3, 4};
  BOOST_TEST(down(xs, 1) == (Coordinates{1, 1, 3, 4}));
}

BOOST_AUTO_TEST_CASE(test_up_wsize_open_ok) {
  Coordinates xs{1, 2, 2, 3};
  Sizes sizes{2, 4, 4, 4};
  vector<BoundaryCondition> bcs{
      BoundaryCondition::OPEN,
      BoundaryCondition::OPEN,
      BoundaryCondition::OPEN,
      BoundaryCondition::OPEN,
  };

  BOOST_TEST(up(xs, sizes, bcs, 2) == (Coordinates{1, 2, 3, 3}));
}
BOOST_AUTO_TEST_CASE(test_up_wsize_open_wrap_fail) {
  Coordinates xs{1, 2, 2, 3};
  Sizes sizes{2, 4, 4, 4};
  vector<BoundaryCondition> bcs{
      BoundaryCondition::OPEN,
      BoundaryCondition::OPEN,
      BoundaryCondition::OPEN,
      BoundaryCondition::OPEN,
  };

  BOOST_TEST(up(xs, sizes, bcs, 3) == (Coordinates{}));
}
BOOST_AUTO_TEST_CASE(test_up_wsize_periodic_wrap) {
  Coordinates xs{1, 2, 2, 3};
  Sizes sizes{2, 4, 4, 4};
  vector<BoundaryCondition> bcs{
      BoundaryCondition::PERIODIC,
      BoundaryCondition::PERIODIC,
      BoundaryCondition::PERIODIC,
      BoundaryCondition::PERIODIC,
  };

  BOOST_TEST(up(xs, sizes, bcs, 3) == (Coordinates{1, 2, 2, 0}));
}

BOOST_AUTO_TEST_CASE(test_down_wsize_open_ok) {
  Coordinates xs{1, 2, 2, 3};
  Sizes sizes{2, 4, 4, 4};
  vector<BoundaryCondition> bcs{
      BoundaryCondition::OPEN,
      BoundaryCondition::OPEN,
      BoundaryCondition::OPEN,
      BoundaryCondition::OPEN,
  };

  BOOST_TEST(down(xs, sizes, bcs, 2) == (Coordinates{1, 2, 1, 3}));
}
BOOST_AUTO_TEST_CASE(test_down_wsize_open_wrap_fail) {
  Coordinates xs{1, 2, 0, 3};
  Sizes sizes{2, 4, 4, 4};
  vector<BoundaryCondition> bcs{
      BoundaryCondition::OPEN,
      BoundaryCondition::OPEN,
      BoundaryCondition::OPEN,
      BoundaryCondition::OPEN,
  };

  BOOST_TEST(down(xs, sizes, bcs, 2) == (Coordinates{}));
}
BOOST_AUTO_TEST_CASE(test_down_wsize_periodic_wrap) {
  Coordinates xs{1, 2, 0, 3};
  Sizes sizes{2, 4, 4, 4};
  vector<BoundaryCondition> bcs{
      BoundaryCondition::PERIODIC,
      BoundaryCondition::PERIODIC,
      BoundaryCondition::PERIODIC,
      BoundaryCondition::PERIODIC,
  };

  BOOST_TEST(down(xs, sizes, bcs, 2) == (Coordinates{1, 2, 3, 3}));
}

BOOST_AUTO_TEST_CASE(test_add_parity) {

  Sizes sizes{7, 7, 7, 7, 7, 7};
  vector<int> nonspatial_dimensions{3, 4};

  SizeParityD expected{
      {7, Parity::EVEN}, //
      {7, Parity::EVEN}, //
      {7, Parity::EVEN}, //
      {7, Parity::NONE}, //
      {7, Parity::NONE}, //
      {7, Parity::EVEN}, //
  };

  BOOST_TEST(add_parity(sizes, nonspatial_dimensions) == expected);
}

BOOST_AUTO_TEST_SUITE_END()
