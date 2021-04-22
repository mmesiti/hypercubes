#define BOOST_TEST_MODULE test_q1D
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>

// We need to use a concrete class for testing.
#include "1D/q1D.hpp"

using namespace hypercubes::slow;
namespace bdata = boost::unit_test::data;

template <class Q1Dtype> struct Q1DFixture {

  Q1Dtype partitioning1D;
  Q1DFixture()
      : partitioning1D(SizeParity{42, Parity::EVEN}, /* sp */
                       0,                            /*dimension*/
                       "test1D",                     /*name*/
                       4 /*nparts*/){};
};

/* Testing all non-virtual public methods in Partitioning1D
 * */
BOOST_AUTO_TEST_SUITE(test_Q1D)

BOOST_DATA_TEST_CASE_F(Q1DFixture<Q1DPeriodic>, test_n_coord_to_idxs_periodic,
                       bdata::xrange(42), i) {
  auto idxs = partitioning1D.coord_to_idxs(i);
  BOOST_TEST(idxs.size() == 3);
}

BOOST_DATA_TEST_CASE_F(Q1DFixture<Q1DOpen>, test_n_coord_to_idxs_open,
                       bdata::xrange(42), i) {

  int expected_copies = (i < 33 and i > 10) ? 3 : 2;
  auto idxs = partitioning1D.coord_to_idxs(i);
  BOOST_TEST(expected_copies == idxs.size());
}

BOOST_FIXTURE_TEST_CASE(test_coord_to_idxs_open, Q1DFixture<Q1DOpen>) {

  auto idxs = partitioning1D.coord_to_idxs(5);
  decltype(idxs) expected_idxs{{0, 5, false}, {1, 5 - 11, true}};
  BOOST_TEST(expected_idxs == idxs);
}

BOOST_FIXTURE_TEST_CASE(test_coord_to_idxs_periodic, Q1DFixture<Q1DPeriodic>) {

  auto idxs = partitioning1D.coord_to_idxs(35);
  decltype(idxs) expected_idxs{
      {2, 35 - 22, true}, {3, 35 - 33, false}, {0, 35 - 42, true}};
  BOOST_TEST(expected_idxs == idxs);
}

BOOST_FIXTURE_TEST_CASE(test_coord_to_idxs_open0, Q1DFixture<Q1DOpen>) {

  auto idxs = partitioning1D.coord_to_idxs(0);
  decltype(idxs) expected_idxs{{0, 0, false}, {1, -11, true}};
  BOOST_TEST(expected_idxs == idxs);
}

BOOST_FIXTURE_TEST_CASE(test_coord_to_idxs_periodic0, Q1DFixture<Q1DPeriodic>) {

  auto idxs = partitioning1D.coord_to_idxs(0);
  decltype(idxs) expected_idxs{{3, 9, true}, {0, 0, false}, {1, -11, true}};
  BOOST_TEST(expected_idxs == idxs);
}

BOOST_FIXTURE_TEST_CASE(test_coord_to_idxs_open41, Q1DFixture<Q1DOpen>) {

  auto idxs = partitioning1D.coord_to_idxs(41);
  decltype(idxs) expected_idxs{{2, 19, true}, {3, 8, false}};
  BOOST_TEST(expected_idxs == idxs);
}

BOOST_FIXTURE_TEST_CASE(test_coord_to_idxs_periodic41,
                        Q1DFixture<Q1DPeriodic>) {

  auto idxs = partitioning1D.coord_to_idxs(41);
  decltype(idxs) expected_idxs{{2, 19, true}, {3, 8, false}, {0, -1, true}};
  BOOST_TEST(expected_idxs == idxs);
}

BOOST_FIXTURE_TEST_CASE(test_max_idx_value, Q1DFixture<Q1DPeriodic>) {
  BOOST_TEST(partitioning1D.max_idx_value() == 4);
}

auto test_idx_to_coord_real_helper = [](auto partitioning1D, int i) {
  auto idxs = partitioning1D.coord_to_idxs(i);
  auto real = *std::find_if(idxs.begin(), idxs.end(),
                            [](auto idx) { return not idx.cached_flag; });
  int offset = real.rest;
  int idx = real.idx;
  int new_i = partitioning1D.idx_to_coord(idx, offset);
  return new_i;
};

BOOST_DATA_TEST_CASE_F(Q1DFixture<Q1DOpen>, test_idx_to_coord_real_open,
                       bdata::xrange(42), i) {
  int new_i = test_idx_to_coord_real_helper(partitioning1D, i);
  BOOST_TEST(new_i == i);
}
BOOST_DATA_TEST_CASE_F(Q1DFixture<Q1DPeriodic>, test_idx_to_coord_real_periodic,
                       bdata::xrange(42), i) {
  int new_i = test_idx_to_coord_real_helper(partitioning1D, i);
  BOOST_TEST(new_i == i);
}
auto test_idx_to_coord_any_helper = [](auto partitioning1D, int i) {
  auto idxs = partitioning1D.coord_to_idxs(i);
  std::vector<int> new_is;
  std::transform(idxs.begin(), idxs.end(), std::back_inserter(new_is),
                 [&](auto i) {
                   int r = partitioning1D.idx_to_coord(i.idx, i.rest);
                   while (r < 0)
                     r += 42;
                   while (r > 41)
                     r -= 42;
                   return r;
                 });
  return new_is;
};

BOOST_DATA_TEST_CASE_F(Q1DFixture<Q1DOpen>, test_idx_to_coord_any_open,
                       bdata::xrange(42), i) {
  auto new_is = test_idx_to_coord_any_helper(partitioning1D, i);
  for (auto new_i : new_is)
    BOOST_TEST(new_i == i);
}
BOOST_DATA_TEST_CASE_F(Q1DFixture<Q1DPeriodic>, test_idx_to_coord_any_periodic,
                       bdata::xrange(42), i) {
  auto new_is = test_idx_to_coord_any_helper(partitioning1D, i);
  for (auto new_i : new_is)
    BOOST_TEST(new_i == i);
}

BOOST_AUTO_TEST_SUITE_END()
