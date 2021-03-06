#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include "partitioners/1D/hbb1D.hpp"
#include "partitioners/1D/plain1D.hpp"
#include "partitioners/1D/q1D.hpp"
#include "partitioners/dimensionalise.hpp"
#include "partitioners/eo_partitioning.hpp"
#include "utils/tuple_printer.hpp"

using namespace hypercubes::slow;
using namespace hypercubes::slow::partitioning;

BOOST_AUTO_TEST_SUITE(test_comments)

BOOST_AUTO_TEST_CASE(test_tuple_printer) {
  BOOST_TEST(tuple_to_str(std::make_tuple(1, 'a', std::string("this thing"))) ==
             "(1, a, this thing)");
}

BOOST_AUTO_TEST_CASE(test_vector_printer) {
  BOOST_TEST(tuple_to_str(std::make_tuple(std::vector<int>{1, 2, 3, 4})) ==
             "([1, 2, 3, 4])");
}

BOOST_AUTO_TEST_CASE(test_parity_printer) {
  BOOST_TEST(tuple_to_str(std::make_tuple(Parity::EVEN)) == "(EVEN)");
}

BOOST_AUTO_TEST_CASE(test_sizeparity_printer) {
  BOOST_TEST(tuple_to_str(std::make_tuple(PartInfo{12, Parity::EVEN})) ==
             "(<12,EVEN>)");
}

// Testing comment strings
BOOST_AUTO_TEST_CASE(test_hbb1D_comment) {
  BOOST_TEST(HBB1D(PartInfo{11, Parity::EVEN}, 1, "hbbtest", 1).comments() ==
             "(11, EVEN, 1, hbbtest, 1)");
}

BOOST_AUTO_TEST_CASE(test_q1Dp_comment) {
  BOOST_TEST(Q1DPeriodic(PartInfo{42, Parity::EVEN}, 1, "QPer", 4).comments() ==
             "(42, EVEN, 1, QPer, 4, PERIODIC)");
}

BOOST_AUTO_TEST_CASE(test_q1Do_comment) {
  BOOST_TEST(Q1DOpen(PartInfo{42, Parity::ODD}, 1, "Qopen", 4).comments() ==
             "(42, ODD, 1, Qopen, 4, OPEN)");
}

BOOST_AUTO_TEST_CASE(test_plain1D_comment) {
  BOOST_TEST(Plain1D(PartInfo{5, Parity::NONE}, 1, "Qopen").comments() ==
             "(5, NONE, 1, Qopen)");
}

BOOST_AUTO_TEST_CASE(test_eop_comment) {
  BOOST_TEST(EO(PartInfos{{5, Parity::EVEN}, {5, Parity::ODD}},
                vector<bool>{true, true}, "EO")
                 .comments() == "([<5,EVEN>, <5,ODD>], TT, EO)");
}

BOOST_AUTO_TEST_SUITE_END()
