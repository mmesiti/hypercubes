#include "bool_maybe.hpp"
#include <boost/test/unit_test.hpp>
#include <sstream>

using namespace hypercubes::slow;

BOOST_AUTO_TEST_SUITE(test_bool_maybe)

BOOST_AUTO_TEST_CASE(test_and_symmetry) {
  BoolM cases[]{BoolM::F, BoolM::M, BoolM::T};

  for (auto a : cases)
    for (auto b : cases)
      BOOST_TEST((a and b) == (b and a));
}

BOOST_AUTO_TEST_CASE(test_or_symmetry) {
  BoolM cases[]{BoolM::F, BoolM::M, BoolM::T};

  for (auto a : cases)
    for (auto b : cases)
      BOOST_TEST((a or b) == (b or a));
}

BOOST_AUTO_TEST_CASE(test_repr) {
  BoolM cases[]{BoolM::F, BoolM::M, BoolM::T};
  char representations[]{'F', 'M', 'T'};
  for (int i = 0; i < 3; ++i) {
    std::stringstream ss;
    ss << cases[i];
    BOOST_TEST(ss.str()[0] == representations[i]);
  }
}

BOOST_AUTO_TEST_SUITE_END()
