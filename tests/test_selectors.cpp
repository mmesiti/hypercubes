#include "selectors/intervals.hpp"
#include "selectors/selectors.hpp"
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <stdexcept>

using namespace hypercubes::slow;
using namespace selectors_v2;

BOOST_AUTO_TEST_SUITE(test_index_interval_map)
BOOST_AUTO_TEST_CASE(test_index_interval_map_constructor_all_known) {

  IndexIntervalMap map({"f", "c", "d", "e"}, {5, 4, 3, 4});
  BOOST_TEST(map._map.size() == 4);
  BOOST_TEST(same(map._map[3].second, Interval(4, 5)));
}
BOOST_AUTO_TEST_CASE(test_index_interval_map_constructor_some_unk) {

  IndexIntervalMap map({"f", "c", "d", "e"}, {5, 4});
  BOOST_TEST(map._map.size() == 4);
  BOOST_TEST(same(map._map[3].second, Interval(0, Interval::LIMIT)));
}
BOOST_AUTO_TEST_CASE(test_index_interval_map_constructor_throws) {
  // if the list of indices is longer than the list of levelnames
  // then constructor must throw invalid_argument

  BOOST_CHECK_THROW(IndexIntervalMap({"f", "c", "d"}, {5, 4, 4, 3}),
                    std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_index_interval_map_access) {

  IndexIntervalMap map({"f", "c", "d", "e"}, {5, 4});
  {
    bool check = same(map["e"], Interval(0, Interval::LIMIT));
    BOOST_TEST(check);
  }
  {
    bool check = same(map["f"], Interval(5, 6));
    BOOST_TEST(check);
  }
}
BOOST_AUTO_TEST_CASE(test_index_interval_map_access_no_exist_throws) {

  IndexIntervalMap map({"f", "c", "d", "e"}, {5, 4});
  BOOST_CHECK_THROW(map["g"], std::invalid_argument);
}
BOOST_AUTO_TEST_SUITE_END()
