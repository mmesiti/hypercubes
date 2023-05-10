#include "selectors/intervals.hpp"
#include "selectors/selectors.hpp"
#include <boost/test/data/test_case.hpp>
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <stdexcept>

using namespace hypercubes::slow;
using namespace selectors_v2;
namespace bdata = boost::unit_test::data;

BOOST_AUTO_TEST_SUITE(test_index_interval_map)
BOOST_AUTO_TEST_CASE(test_index_interval_map_constructor_all_known) {

  IndexIntervalMap map({"f", "c", "d", "e"}, {5, 4, 3, 4});
  BOOST_TEST(map._strings.size() == 4);
  BOOST_TEST(same(map._intervals[3], Interval(4, 5)));
}
BOOST_AUTO_TEST_CASE(test_index_interval_map_constructor_some_unk) {

  IndexIntervalMap map({"f", "c", "d", "e"}, {5, 4});
  BOOST_TEST(map._strings.size() == 4);
  BOOST_TEST(same(map._intervals[3], Interval(0, Interval::LIMIT)));
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

BOOST_DATA_TEST_CASE(test_selector_and, bdata::xrange(10), i) {

  Selector s1 = [](const IndexIntervalMap &map) { return (map["a"] < 5); };
  Selector s2 = [](const IndexIntervalMap &map) { return (map["a"] > 2); };

  auto combination = (s1 and s2);

  IndexIntervalMap map({"a"}, {i});
  bool check = (s1(map) and s2(map)) == combination(map);
  BOOST_TEST(check);
}
BOOST_DATA_TEST_CASE(test_selector_or, bdata::xrange(10), i) {

  Selector s1 = [](const IndexIntervalMap &map) { return (map["a"] > 5); };
  Selector s2 = [](const IndexIntervalMap &map) { return (map["a"] < 2); };

  auto combination = (s1 or s2);

  IndexIntervalMap map({"a"}, {i});
  bool check = (s1(map) or s2(map)) == combination(map);
  BOOST_TEST(check);
}
BOOST_DATA_TEST_CASE(test_selector_not, bdata::xrange(10), i) {

  Selector s = [](const IndexIntervalMap &map) { return (map["a"] > 5); };

  auto negation = not s;

  IndexIntervalMap map({"a"}, {i});
  bool check = (not s(map)) == negation(map);
  BOOST_TEST(check);
}
BOOST_AUTO_TEST_SUITE_END()
