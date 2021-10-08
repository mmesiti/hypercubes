#include "trees/kvtree.hpp"
#include "trees/kvtree_data_structure.hpp"
#include "trees/kvtree_v2.hpp"
#include "trees/tree_data_structure.hpp"
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

using namespace hypercubes::slow::internals;

BOOST_AUTO_TEST_SUITE(test_kvtree_v2)

BOOST_AUTO_TEST_CASE(test_mtkv) {
  KVTreePv2<int> t = mtkv(1, {});
  auto t2 = mtkv(1, {{{2}, t},   //
                     {{3}, t}}); //
}

BOOST_AUTO_TEST_CASE(test_pull_keys_up) {
  // This is a nicer form
  auto t = mt(mp(0, 1), {mt(mp(3, 4), {mt(mp(8, 80), {}),   //
                                       mt(mp(9, 80), {})}), //
                         mt(mp(5, 4), {mt(mp(18, 80), {}),  //
                                       mt(mp(19, 80), {})})});

  auto t_kv = pull_keys_up(t);
  // This is a longer form
  auto t_kv_exp = mtkv(1, {{{3},
                            mtkv(4, {{{8}, mtkv(80, {})},    //
                                     {{9}, mtkv(80, {})}})}, //
                           {{5},
                            mtkv(4, {{{18}, mtkv(80, {})},      //
                                     {{19}, mtkv(80, {})}})}}); //

  BOOST_TEST(*t_kv == *t_kv_exp);
}
BOOST_AUTO_TEST_CASE(test_push_keys_down) {
  auto t = mt(mp(0, 1), {mt(mp(3, 4), {mt(mp(8, 80), {}),   //
                                       mt(mp(9, 80), {})}), //
                         mt(mp(5, 4), {mt(mp(18, 80), {}),  //
                                       mt(mp(19, 80), {})})});
  auto t_kv = pull_keys_up(t);
  auto t_again = push_keys_down(t_kv);
  BOOST_TEST(*t_again == *t);
}

BOOST_AUTO_TEST_CASE(test_collapse_level_by_index) {
  auto _t = mt(mp(0, 1), {mt(mp(3, 2), {mt(mp(5, 3), {mt(mp(1, 4), {}),     //
                                                      mt(mp(2, 5), {})}),   //
                                        mt(mp(6, 6), {mt(mp(3, 7), {}),     //
                                                      mt(mp(4, 8), {})})}), //
                          mt(mp(4, 9), {mt(mp(7, 10), {mt(mp(5, 11), {}),   //
                                                       mt(mp(7, 12), {})}), //
                                        mt(mp(8, 13), {mt(mp(8, 14), {}),   //
                                                       mt(mp(9, 15), {})})})});

  auto _tcollapsed_exp = mt(mp(0, 1), {mt(mp(3, 3), {mt(mp(1, 4), {}),   //
                                                     mt(mp(2, 5), {})}), //
                                       mt(mp(4, 10), {mt(mp(5, 11), {}), //
                                                      mt(mp(7, 12), {})})});

  auto t = pull_keys_up(_t);
  auto tcollapsed_exp = pull_keys_up(_tcollapsed_exp);

  auto tcollapsed = collapse_level_by_index(t,                       //
                                            1 /*level to collapse*/, //
                                            0 /*child to replace*/);

  BOOST_TEST(*tcollapsed_exp == *tcollapsed);
}

BOOST_AUTO_TEST_CASE(test_collapse_level_by_key_all_there) {

  auto _t = mt(mp(0, 1), {mt(mp(3, 2), {mt(mp(5, 3), {mt(mp(1, 4), {}),     //
                                                      mt(mp(2, 5), {})}),   //
                                        mt(mp(6, 6), {mt(mp(3, 7), {}),     //
                                                      mt(mp(4, 8), {})})}), //
                          mt(mp(4, 9), {mt(mp(5, 10), {mt(mp(5, 11), {}),   //
                                                       mt(mp(7, 12), {})}), //
                                        mt(mp(8, 13), {mt(mp(8, 14), {}),   //
                                                       mt(mp(9, 15), {})})})});

  auto _tcollapsed_exp = mt(mp(0, 1), {mt(mp(3, 3), {mt(mp(1, 4), {}),   //
                                                     mt(mp(2, 5), {})}), //
                                       mt(mp(4, 10), {mt(mp(5, 11), {}), //
                                                      mt(mp(7, 12), {})})});

  auto t = pull_keys_up(_t);
  auto tcollapsed_exp = pull_keys_up(_tcollapsed_exp);

  auto tcollapsed = collapse_level_by_key(t,                       //
                                          1 /*level to collapse*/, //
                                          {5} /*child key to replace*/);

  BOOST_TEST(*tcollapsed_exp == *tcollapsed);
}
BOOST_AUTO_TEST_CASE(test_collapse_level_by_key_missing) {

  auto _t = mt(mp(0, 1), {mt(mp(3, 2), {mt(mp(5, 3), {mt(mp(1, 4), {}),     //
                                                      mt(mp(2, 5), {})}),   //
                                        mt(mp(6, 6), {mt(mp(3, 7), {}),     //
                                                      mt(mp(4, 8), {})})}), //
                          mt(mp(4, 9), {mt(mp(6, 10), {mt(mp(5, 11), {}),   //
                                                       mt(mp(7, 12), {})}), //
                                        mt(mp(8, 13), {mt(mp(8, 14), {}),   //
                                                       mt(mp(9, 15), {})})})});

  auto _tcollapsed_exp = mt(mp(0, 1), {mt(mp(3, 3), {mt(mp(1, 4), {}), //
                                                     mt(mp(2, 5), {})})});
  auto t = pull_keys_up(_t);
  auto tcollapsed_exp = pull_keys_up(_tcollapsed_exp);

  auto tcollapsed = collapse_level_by_key(t,                       //
                                          1 /*level to collapse*/, //
                                          {5} /*child key to replace*/);

  BOOST_TEST(*tcollapsed_exp == *tcollapsed);
}

BOOST_AUTO_TEST_CASE(test_bring_level_on_top_key) {
  auto _t =
      mt(mp(0, 1), {mt(mp(20, 2), {mt(mp(22, 3), {mt(mp(24, 4), {}), //
                                                  mt(mp(25, 5), {})})}),
                    mt(mp(21, 9), {mt(mp(23, 3), {mt(mp(24, 11), {}), //
                                                  mt(mp(26, 12), {})})})});

  auto _transf_expected =
      mt(mp(0, 3), {mt(mp(24, 1), {mt(mp(20, 2), {mt(mp(22, 4), {})}), //
                                   mt(mp(21, 9), {mt(mp(23, 11), {})})}),
                    mt(mp(25, 1), {mt(mp(20, 2), {mt(mp(22, 5), {})}), //
                                   mt(mp(21, 9), {})}),                //
                    mt(mp(26, 1), {mt(mp(20, 2), {}),                  //
                                   mt(mp(21, 9), {mt(mp(23, 12), {})})})});
  auto t = pull_keys_up(_t);
  auto transf_expected = pull_keys_up(_transf_expected);

  auto transf = bring_level_on_top_by_key(t, 2);

  BOOST_TEST(*transf_expected == *transf);
}

BOOST_AUTO_TEST_CASE(test_bring_level_on_top_key_throws) {
  auto _t =
      mt(mp(0, 1),
         {mt(mp(20, 2), {mt(mp(22, 3 /*different*/), {mt(mp(24, 4), {}), //
                                                      mt(mp(25, 5), {})})}),
          mt(mp(21, 9), {mt(mp(23, 4 /*different*/), {mt(mp(24, 11), {}), //
                                                      mt(mp(26, 12), {})})})});

  auto t = pull_keys_up(_t);
  BOOST_CHECK_THROW(bring_level_on_top_by_key(t, 2), TreeLevelPermutationError);
}

BOOST_AUTO_TEST_CASE(test_swap_levels_by_key) {

  auto _t =
      mt(mp(0, 1), {mt(mp(10, 2), {mt(mp(12, 3), {mt(mp(14, 5), {}),     //
                                                  mt(mp(15, 6), {})})}), //
                    mt(mp(11, 4), {mt(mp(13, 3), {mt(mp(14, 7), {}),     //
                                                  mt(mp(15, 8), {})})})});
  vector<int> new_level_ordering{2, 0, 1};

  auto _newt_exp =
      mt(mp(0, 3), {mt(mp(14, 1), {mt(mp(10, 2), {mt(mp(12, 5), {})}),   //
                                   mt(mp(11, 4), {mt(mp(13, 7), {})})}), //
                    mt(mp(15, 1), {mt(mp(10, 2), {mt(mp(12, 6), {})}),   //
                                   mt(mp(11, 4), {mt(mp(13, 8), {})})})});

  auto t = pull_keys_up(_t);
  auto newt_exp = pull_keys_up(_newt_exp);

  auto newt = swap_levels(t, new_level_ordering);

  BOOST_TEST(*newt == *newt_exp);
}

BOOST_AUTO_TEST_SUITE_END()
