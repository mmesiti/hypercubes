#include "api_v2/level_names.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
using std::vector;
using namespace hypercubes::slow::internals;
BOOST_AUTO_TEST_SUITE(test_level_names)
BOOST_AUTO_TEST_CASE(test_insert_new_level_name) {
  vector<LevelName> levels{{0}, {1}, {2}, {3}};
  auto new_levels = insert_new_level_name(levels, 1);
  vector<LevelName> exp_levels{{0}, {4}, {1}, {2}, {3}};

  BOOST_TEST(new_levels == exp_levels);
}

BOOST_AUTO_TEST_CASE(test_flatten_levels) {

  vector<LevelName> levels{{0}, {1}, {2}, {3}};
  int levelstart = 1;
  int levelend = 3;
  vector<LevelName> exp_levels{{0}, {4}, {3}};
  auto new_levels = flatten_levels(levels, levelstart, levelend);
  BOOST_TEST(new_levels == exp_levels);
}

BOOST_AUTO_TEST_CASE(test_find_level) {

  vector<LevelName> levels{{0}, {2}, {3}, {1}};
  BOOST_TEST(find_level({1}, levels) == 3);
}

BOOST_AUTO_TEST_SUITE_END()
