#include "api_v2/transform_network.hpp"
#include "api_v2/transform_request_makers.hpp"
#include "api_v2/transform_requests.hpp"
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>

using namespace hypercubes::slow::internals;
BOOST_AUTO_TEST_SUITE(test_transform_request_makers)
// Testing boilerplate - only that the right type of ptr is returned in all
// cases.
BOOST_AUTO_TEST_CASE(test_id) {
  auto t = trms::Id({4, 4}, {"X", "Y"}, "ROOT");
  auto p = std::dynamic_pointer_cast<transform_requests::Id>(t);
  bool check = p != NULL;
  BOOST_TEST(check);
}
BOOST_AUTO_TEST_CASE(test_renumber) {
  auto t = trms::Renumber("TEST");
  auto p = std::dynamic_pointer_cast<transform_requests::Renumber>(t);
  bool check = p != NULL;
  BOOST_TEST(check);
}
BOOST_AUTO_TEST_CASE(test_q) {
  auto t = trms::Q("TESTLEVEL", 4, "TESTNEWLEVEL", "TESTNODE");
  auto p = std::dynamic_pointer_cast<transform_requests::Q>(t);
  bool check = p != NULL;
  BOOST_TEST(check);
}
BOOST_AUTO_TEST_CASE(test_bb) {
  auto t = trms::BB("TESTLEVEL", 1, "TESTNEWLEVEL", "TESTNODE");
  auto p = std::dynamic_pointer_cast<transform_requests::BB>(t);
  bool check = p != NULL;
  BOOST_TEST(check);
}
BOOST_AUTO_TEST_CASE(test_flatten) {
  auto t = trms::Flatten("TESTLEVELSTART", "TESTLEVELENED", "TESTNEWLEVEL",
                         "TESTNODE");
  auto p = std::dynamic_pointer_cast<transform_requests::Flatten>(t);
  bool check = p != NULL;
  BOOST_TEST(check);
}
BOOST_AUTO_TEST_CASE(test_collect_leaves) {
  auto t = trms::CollectLeaves("TESTLEVELSTART", "TESTNEWLEVEL", 4, "TESTNODE");
  auto p = std::dynamic_pointer_cast<transform_requests::CollectLeaves>(t);
  bool check = p != NULL;
  BOOST_TEST(check);
}
BOOST_AUTO_TEST_CASE(test_level_remap) {
  auto t = trms::LevelRemap("TESTLEVELSTART", {1, 2, 3}, "TESTNODE");
  auto p = std::dynamic_pointer_cast<transform_requests::LevelRemap>(t);
  bool check = p != NULL;
  BOOST_TEST(check);
}
BOOST_AUTO_TEST_CASE(test_level_swap1) {
  auto t = trms::LevelSwap({"L1", "L2", "L3"}, "TESTNODE");
  auto p = std::dynamic_pointer_cast<transform_requests::LevelSwap1>(t);
  bool check = p != NULL;
  BOOST_TEST(check);
}
BOOST_AUTO_TEST_CASE(test_level_swap2) {
  auto t = trms::LevelSwap({"L1", "L2", "L3"}, {"L1", "L2", "L3"}, "TESTNODE");
  auto p = std::dynamic_pointer_cast<transform_requests::LevelSwap2>(t);
  bool check = p != NULL;
  BOOST_TEST(check);
}
BOOST_AUTO_TEST_CASE(test_eo_naive) {
  auto t = trms::EONaive("FLATTENED", "EO", "TESTNODE");
  auto p = std::dynamic_pointer_cast<transform_requests::EONaive>(t);
  bool check = p != NULL;
  BOOST_TEST(check);
}
BOOST_AUTO_TEST_CASE(test_sum) {
  auto t = trms::Sum("NLN", {}, "TESTNODE");
  auto p = std::dynamic_pointer_cast<transform_requests::Sum>(t);
  bool check = p != NULL;
  BOOST_TEST(check);
}
BOOST_AUTO_TEST_CASE(Fork) {
  auto t = trms::Fork({});
  auto p = std::dynamic_pointer_cast<transform_requests::Fork>(t);
  bool check = p != NULL;
  BOOST_TEST(check);
}
BOOST_AUTO_TEST_CASE(TreeComposition) {
  auto t = trms::TreeComposition({}, "ENDNODE");
  auto p = std::dynamic_pointer_cast<transform_requests::TreeComposition>(t);
  bool check = p != NULL;
  BOOST_TEST(check);
}
BOOST_AUTO_TEST_SUITE_END()
