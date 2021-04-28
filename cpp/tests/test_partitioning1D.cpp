#include <boost/test/data/test_case.hpp>
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>

// We need to use a concrete class for testing.
#include "1D/q1D.hpp"
#include <vector>

using namespace hypercubes::slow;

struct Q1DFixture {

  Q1DPeriodic partitioning1D;

  Q1DFixture()
      : partitioning1D(SizeParity{42, Parity::EVEN}, /* sp */
                       0,                            /*dimension*/
                       "test1D",                     /*name*/
                       4 /*nparts*/){};
};

/* Testing all non-virtual public methods in Partitioning1D
 * */
BOOST_AUTO_TEST_SUITE(test_partitioning1D)

BOOST_FIXTURE_TEST_CASE(test_starts, Q1DFixture) {

  std::vector<int> exp_starts{0, 11, 22, 33};
  for (int i = 0; i < exp_starts.size(); ++i)
    BOOST_TEST(exp_starts[i] == partitioning1D.start(i));
}

BOOST_FIXTURE_TEST_CASE(test_ends, Q1DFixture) {
  std::vector<int> exp_ends{11, 22, 33, 42};
  for (int i = 0; i < exp_ends.size(); ++i)
    BOOST_TEST(exp_ends[i] == partitioning1D.end(i));
}

BOOST_FIXTURE_TEST_CASE(test_sizeparity_info_list, Q1DFixture) {

  std::vector<SizeParity> exp_sp{
      {9, Parity::ODD}, {11, Parity::EVEN}, {11, Parity::ODD}};
  auto res = partitioning1D.sub_sizeparity_info_list();
  BOOST_TEST(exp_sp == res);
}

namespace bdata = boost::unit_test::data;
BOOST_DATA_TEST_CASE_F(Q1DFixture, test_idx_to_child_kind, bdata::xrange(4),
                       i) {
  int child_kind;
  if (i == 0 or i == 2)
    child_kind = 1;
  else if (i == 1)
    child_kind = 2;
  else
    child_kind = 0;

  BOOST_TEST(child_kind == partitioning1D.idx_to_child_kind(i));
}

namespace bdata = boost::unit_test::data;
BOOST_DATA_TEST_CASE_F(Q1DFixture, test_idx_to_size, bdata::xrange(4), i) {
  int size;
  if (i == 0 or i == 1 or i == 2)
    size = 11;
  else
    size = 9;

  BOOST_TEST(size == partitioning1D.idx_to_size(i));
}

BOOST_FIXTURE_TEST_CASE(test_idx_to_child_kind_throws, Q1DFixture) {
  BOOST_CHECK_THROW(partitioning1D.idx_to_child_kind(55),
                    std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()
