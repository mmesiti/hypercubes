#include <boost/test/data/test_case.hpp>
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <stdexcept>

#include "1D/plain1D.hpp"

using namespace hypercubes::slow;
namespace bdata = boost::unit_test::data;

struct Plain1DFixture {

  Plain1D partitioning1D;
  Plain1DFixture()
      : partitioning1D(SizeParity{7, Parity::EVEN}, /* sp */
                       0,                           /*dimension*/
                       "test1D")                    /*name*/
  {}
};

/* Testing all non-virtual public methods in Partitioning1D
 * */
BOOST_AUTO_TEST_SUITE(test_HBB1D)

BOOST_FIXTURE_TEST_CASE(test_starts, Plain1DFixture) {
  std::vector<int> expected_starts{0};

  for (int i = 0; i < expected_starts.size(); ++i)
    BOOST_TEST(expected_starts[i] == partitioning1D.start(i));
}
BOOST_FIXTURE_TEST_CASE(test_ends, Plain1DFixture) {
  std::vector<int> expected_ends{1};

  for (int i = 0; i < expected_ends.size(); ++i)
    BOOST_TEST(expected_ends[i] == partitioning1D.end(i));
}

BOOST_DATA_TEST_CASE_F(Plain1DFixture, test_coord_to_idxs_size,
                       bdata::xrange(-3, 10), relative_x) {
  auto idxs = partitioning1D.coord_to_idxs(relative_x);
  int exp_size = (relative_x >= 0 && relative_x < 7) ? 1 : 0;
  BOOST_TEST(idxs.size() == exp_size);
}
BOOST_DATA_TEST_CASE_F(Plain1DFixture, test_coords_to_idxs, bdata::xrange(0, 7),
                       i) {
  auto r = partitioning1D.coord_to_idxs(i)[0];
  BOOST_TEST(r.idx == i);
  BOOST_TEST(r.rest == 0);
}
BOOST_DATA_TEST_CASE_F(Plain1DFixture, test_idx_to_coord, bdata::xrange(0, 7),
                       i) {
  auto r = partitioning1D.idx_to_coord(i, 0);
  BOOST_TEST(r == i);
}
BOOST_FIXTURE_TEST_CASE(test_idx_to_coord_throws_woffset, Plain1DFixture) {
  BOOST_CHECK_THROW(partitioning1D.idx_to_coord(2, 1), std::invalid_argument);
}
BOOST_FIXTURE_TEST_CASE(test_idx_to_coord_throws_outofrange, Plain1DFixture) {
  BOOST_CHECK_THROW(partitioning1D.idx_to_coord(8, 0), std::invalid_argument);
}

BOOST_FIXTURE_TEST_CASE(test_max_idx_value, Plain1DFixture) {
  BOOST_TEST(partitioning1D.max_idx_value() == 7);
}

BOOST_FIXTURE_TEST_CASE(test_sub_sizeparity_info_list, Plain1DFixture) {
  BOOST_TEST(partitioning1D.sub_sizeparity_info_list().size() == 2);
}

BOOST_AUTO_TEST_CASE(test_sub_sizeparity_info_list_paritynone) {

  Plain1D partitioning1D(SizeParity{7, Parity::NONE}, /* sp */
                         0,                           /*dimension*/
                         "test1D");                   /*name*/

  BOOST_TEST(partitioning1D.sub_sizeparity_info_list().size() == 1);
}
BOOST_AUTO_TEST_CASE(test_sub_sizeparity_info_list_zerosize) {

  Plain1D partitioning1D(SizeParity{0, Parity::EVEN}, /* sp */
                         0,                           /*dimension*/
                         "test1D");                   /*name*/
  BOOST_TEST(partitioning1D.sub_sizeparity_info_list().size() == 0);
}
BOOST_AUTO_TEST_SUITE_END()
