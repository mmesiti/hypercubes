#include "partitioners/partitioners.hpp"
#include <boost/test/unit_test.hpp>

using namespace hypercubes::slow;

BOOST_AUTO_TEST_SUITE(test_partitioners)
BOOST_AUTO_TEST_CASE(test_eo_partitioner) {

  IPartRP p = partitioner_makers::EO("test_eo", {true, false, true});

  PartInfos sps{{5, Parity::EVEN}, {6, Parity::NONE}, {7, Parity::ODD}};
  auto pc = p->partition(sps);
  std::string expected("([<5,EVEN>, <6,NONE>, <7,ODD>], TFT, test_eo)");
  BOOST_TEST(pc->comments() == expected);
}
BOOST_AUTO_TEST_CASE(test_qper_partitioner) {

  IPartRP p = partitioner_makers::QPeriodic("test_qper", 1, 4);

  PartInfos sps{{42, Parity::EVEN}, {42, Parity::NONE}, {42, Parity::ODD}};
  auto pc = p->partition(sps);
  std::string expected("(42, NONE, 1, test_qper, 4, PERIODIC)");
  BOOST_TEST(pc->comments() == expected);
}
BOOST_AUTO_TEST_CASE(test_qopen_partitioner) {

  IPartRP p = partitioner_makers::QOpen("test_qper", 1, 4);

  PartInfos sps{{42, Parity::EVEN}, {42, Parity::NONE}, {42, Parity::ODD}};
  auto pc = p->partition(sps);
  std::string expected("(42, NONE, 1, test_qper, 4, OPEN)");
  BOOST_TEST(pc->comments() == expected);
}
BOOST_AUTO_TEST_CASE(test_plain_partitioner) {

  IPartRP p = partitioner_makers::Plain("test_plain", 1);

  PartInfos sps{{42, Parity::EVEN}, {42, Parity::NONE}, {42, Parity::ODD}};
  auto pc = p->partition(sps);
  std::string expected("(42, NONE, 1, test_plain)");
  BOOST_TEST(pc->comments() == expected);
}
BOOST_AUTO_TEST_CASE(test_hbb_partitioner) {

  IPartRP p = partitioner_makers::HBB("test_hbb", 1, 3);

  PartInfos sps{{42, Parity::EVEN}, {42, Parity::NONE}, {42, Parity::ODD}};
  auto pc = p->partition(sps);
  std::string expected("(42, NONE, 1, test_hbb, 3)");
  BOOST_TEST(pc->comments() == expected);
}
BOOST_AUTO_TEST_SUITE_END()
