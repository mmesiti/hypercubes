#include "trees/kvtree.hpp"
#include "trees/kvtree_data_structure.hpp"
#include "trees/tree_data_structure.hpp"
#include <boost/test/unit_test.hpp>

using namespace hypercubes::slow::internals;

BOOST_AUTO_TEST_SUITE(test_kvtree_v2)

BOOST_AUTO_TEST_CASE(test_pull_keys_up) {
  auto t = mt(mp(0, 1), {mt(mp(3, 4), {mt(mp(8, 80), {}),   //
                                       mt(mp(9, 80), {})}), //
                         mt(mp(5, 4), {mt(mp(18, 80), {}),  //
                                       mt(mp(19, 80), {})})});

  auto t_kv = pull_keys_up(t);
  auto t_kv_exp = mtkv(1, {{3, mtkv(4, {mp(8, mtkv(80, {})),       //
                                        mp(9, mtkv(80, {}))})},    //
                           {5, mtkv(4, {mp(18, mtkv(80, {})),      //
                                        mp(19, mtkv(80, {}))})}}); //
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

BOOST_AUTO_TEST_SUITE_END()
