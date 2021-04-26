#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include "1D/hbb1D.hpp"
#include "1D/plain1D.hpp"
#include "1D/q1D.hpp"
#include "dimensionalise.hpp"
#include "eo_partitioning.hpp"
#include "tuple_printer.hpp"

using namespace hypercubes::slow;
namespace bdata = boost::unit_test::data;

BOOST_AUTO_TEST_SUITE(test_comments)

BOOST_AUTO_TEST_CASE(test_tuple_printer) {

  auto res = tuple_to_str(std::make_tuple(1, 'a', std::string("this thing")));
  std::string expected = "(1, a, this thing)";

  BOOST_TEST(res == expected);
}

BOOST_AUTO_TEST_SUITE_END()
