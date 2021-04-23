#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include "1D/hbb1D.hpp"

using namespace hypercubes::slow;
namespace bdata = boost::unit_test::data;

struct HBB1DFixture {

  HBB1D partitioning1D;
  HBB1DFixture()
      : partitioning1D(SizeParity{7, Parity::EVEN}, /* sp */
                       0,                           /*dimension*/
                       "test1D",                    /*name*/
                       1 /*halo*/) {}
};

/* Testing all non-virtual public methods in Partitioning1D
 * */
BOOST_AUTO_TEST_SUITE(test_HBB1D)

BOOST_FIXTURE_TEST_CASE(test_starts, HBB1DFixture) {
  std::vector<int> expected_starts{-1, 0, 1, 6, 7};

  for (int i = 0; i < expected_starts.size(); ++i)
    BOOST_TEST(expected_starts[i] == partitioning1D.start(i));
}
BOOST_FIXTURE_TEST_CASE(test_ends, HBB1DFixture) {
  std::vector<int> expected_ends{0, 1, 6, 7, 8};

  for (int i = 0; i < expected_ends.size(); ++i)
    BOOST_TEST(expected_ends[i] == partitioning1D.end(i));
}

BOOST_DATA_TEST_CASE_F(HBB1DFixture, test_coord_to_idxs_size,
                       bdata::xrange(-3, 10), relative_x) {
  auto idxs = partitioning1D.coord_to_idxs(relative_x);
  int exp_size = (relative_x >= 0 - 1 && relative_x < 7 + 1) ? 1 : 0;
  BOOST_TEST(idxs.size() == exp_size);
}

BOOST_FIXTURE_TEST_CASE(test_coord_to_idxs, HBB1DFixture) {
  auto idxs = partitioning1D.coord_to_idxs(5);
  decltype(idxs) expected_idxs{{2, 5 - 1, false}};
  BOOST_TEST(expected_idxs == idxs);
}

auto test_idx_to_coord_helper = [](auto partitioning1D, int i) {
  auto r = partitioning1D.coord_to_idxs(i)[0];
  int offset = r.rest;
  int idx = r.idx;
  int new_i = partitioning1D.idx_to_coord(idx, offset);
  return new_i;
};

BOOST_DATA_TEST_CASE_F(HBB1DFixture, test_idx_to_coords, bdata::xrange(-1, 8),
                       i) {
  int new_i = test_idx_to_coord_helper(partitioning1D, i);
  BOOST_TEST(new_i == i);
}

BOOST_FIXTURE_TEST_CASE(test_max_idx_value, HBB1DFixture) {
  BOOST_TEST(partitioning1D.max_idx_value() == 5);
}

BOOST_AUTO_TEST_SUITE_END()
