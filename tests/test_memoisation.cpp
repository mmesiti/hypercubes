#include "trees/kvtree.hpp"
#include "trees/memoisation/memoisation.hpp"
#include "trees/partition_tree.hpp"
#include "trees/tree.hpp"
#include "trees/tree_data_structure.hpp"
#include <boost/test/unit_test.hpp>
#include <functional>
#include <iostream>

int fib(std::function<int(int)> f, int i) {
  if (i == 0)
    return 0;
  if (i == 1)
    return 1;
  return f(i - 1) + f(i - 2);
}
using namespace hypercubes::slow;
using namespace hypercubes::slow::internals;

BOOST_AUTO_TEST_SUITE(test_memoisation)

BOOST_AUTO_TEST_CASE(fibonacci) {

  Memoiser<int, int> R(fib);

  for (int i = 0; i < 10; ++i)
    std::cout << R(i) << std::endl;
}

BOOST_AUTO_TEST_CASE(test_truncate_tree) {
  auto t = mt(1, {
                     mt(2, {mt(3, {mt(4, {}),   //
                                   mt(5, {})}), //
                            mt(6, {mt(7, {}),   //
                                   mt(8, {})})}),
                     mt(9, {mt(10, {mt(11, {}),   //
                                    mt(12, {})}), //
                            mt(13, {mt(14, {}),   //
                                    mt(15, {})})}),
                 });

  BOOST_TEST(*truncate_treeM(t, 3) == *truncate_tree(t, 3));
}

BOOST_AUTO_TEST_CASE(test_get_leaves_list) {
  auto t = mt(1, {
                     mt(2, {mt(3, {mt(4, {}),   //
                                   mt(5, {})}), //
                            mt(6, {mt(7, {}),   //
                                   mt(8, {})})}),
                     mt(9, {mt(10, {mt(11, {}),   //
                                    mt(12, {})}), //
                            mt(13, {mt(14, {}),   //
                                    mt(15, {})})}),
                 });

  BOOST_TEST(get_leaves_listM(t) == get_leaves_list(t));
}

std::string arbitrary_transform(int i) {
  std::stringstream ss;
  ss << "N: ";
  ss << i * 3;
  return ss.str();
}

BOOST_AUTO_TEST_CASE(test_nodemap) {

  TreeP<int> t = mt(1, {mt(2, {}),                     //
                        mt(3, {mt(4, {}), mt(5, {})}), //
                        mt(6, {mt(7, {mt(8, {})})})});

  auto newt = nodemap<int, std::string, arbitrary_transform>(t);
  auto newtM = nodemapM<int, std::string, arbitrary_transform>(t);

  BOOST_TEST(*newt == *newtM);
}

BOOST_AUTO_TEST_CASE(test_number_children) {
  auto tnoidx = mt(1, {mt(2, {mt(3, {mt(5, {}),     //
                                     mt(6, {})})}), //
                       mt(4, {mt(3, {mt(7, {}),     //
                                     mt(8, {})})})});

  auto t = number_children(tnoidx);
  auto tM = number_childrenM(tnoidx);
  BOOST_TEST(*tM == *t);
}

BOOST_AUTO_TEST_CASE(test_prune_tree) {

  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };
  auto tfull =
      mt(mp(0, 1), {mt(mp(0, 2), {mt(mp(0, 3), {mt(mp(0, 5), {}),     //
                                                mt(mp(1, 6), {})})}), //
                    mt(mp(1, 4), {mt(mp(10, 3), {mt(mp(0, 7), {}),    //
                                                 mt(mp(1, 8), {})})})});

  auto predicate = [](vector<int> idxs) {
    return idxs.size() < 1 or idxs[0] == 1;
  };

  auto t = prune_tree(tfull, predicate);
  auto tM = prune_treeM(tfull, predicate);
  BOOST_TEST(*t == *tM);
}

BOOST_AUTO_TEST_CASE(test_get_partition_tree_via_maxidx) {

  using namespace hypercubes::slow::partitioner_makers;
  SizeParityD sp{{42, Parity::EVEN}};
  PartitionTree t = get_partition_tree(sp, //
                                       PartList{
                                           QPeriodic("MPI", 0, 4), //
                                           QOpen("Vector", 0, 2),  //
                                           Plain("Remainder", 0),
                                           Site(),
                                       });
  PartitionTree tM = get_partition_treeM(sp, //
                                         PartList{
                                             QPeriodic("MPI", 0, 4), //
                                             QOpen("Vector", 0, 2),  //
                                             Plain("Remainder", 0),
                                             Site(),
                                         });
  auto tmax = get_max_idx_tree(t);
  auto tmaxM = get_max_idx_treeM(tM);
  BOOST_TEST(*tmax == *tmaxM);
}

BOOST_AUTO_TEST_SUITE_END()
