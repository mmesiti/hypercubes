#define BOOST_TEST_MODULE rilist_test
#include <algorithm>
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>
#include <set>

#include "utils.hpp"

BOOST_AUTO_TEST_SUITE(rilist)

BOOST_AUTO_TEST_CASE(test_rilist_edge_cases_first) {

  data::rilist generator(1, 10, 2, 3);
  auto it = generator.begin();

  std::set<std::vector<int>> results;
  int sequence_elements = 2 * 2 + 2 * 2 * 2;
  for (int i = 0; i < sequence_elements; ++i, ++it)
    results.insert(*it);

  BOOST_TEST(results.size() == sequence_elements);

  for (auto &el : results)
    for (auto &i : el)
      for (int a = 2; a < 10; ++a)
        BOOST_TEST(i != a);
}

namespace bdata = boost::unit_test::data;
BOOST_DATA_TEST_CASE(test_rilist_limits,
                     data::rilist(1, 10, 1, 6) ^ bdata::xrange(1000), sizes,
                     count) {

  bool success = std::all_of(sizes.begin(), sizes.end(),              //
                             [](int i) { return i >= 1 && i <= 10; }) //
                 and sizes.size() <= 10                               //
                 and sizes.size() >= 1;                               //
  if (not success)
    std::cout << sizes;

  BOOST_TEST(success);
}

BOOST_AUTO_TEST_SUITE_END()
