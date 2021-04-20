#define BOOST_TEST_MODULE eo
#include <boost/test/included/unit_test.hpp>

#include "eo.hpp"
#include "random_int_list.hpp"
#include "utils.hpp"
#include <iostream>
#include <random>

namespace std {

std::ostream &operator<<(std::ostream &os, std::vector<int> const &v) {
  os << "{";
  for (const auto &x : v) {
    os << x << ", ";
  }
  os << "}\n";

  return os;
};

} // namespace std
BOOST_AUTO_TEST_SUITE(eo)

namespace bdata = boost::unit_test::data;

BOOST_DATA_TEST_CASE(test_of_utils,
                     (data::minmax(1, 10) + bdata::random(1, 10)) ^
                         bdata::xrange(10),
                     val, idx) {

  std::cout << val << " " << idx << std::endl;
  BOOST_TEST(true);
}

BOOST_DATA_TEST_CASE(test_of_utils_2,
                     data::rilist(1, 10, 1, 6) ^ bdata::xrange(10), val, idx) {

  std::cout << idx << std::endl;
  BOOST_TEST(true);
}

BOOST_AUTO_TEST_SUITE_END()
