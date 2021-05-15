#include "fixtures.hpp"
#include "partition_predicates.hpp"
#include "partition_tree.hpp"
#include "partition_tree_allocations.hpp"
#include "partitioners.hpp"
#include "test_utils.hpp"
#include "tree.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

using namespace hypercubes::slow;
using namespace hypercubes::slow::partitioners;

struct Simple1D {
  PTBuilder tree_builder;
  PartList partitioners;
  SizeParityD spd;
  PartitionTree t;
  TreeP<std::pair<int, int>> max_idx_tree;
  TreeP<std::pair<int, int>> sizetree;

  Simple1D()
      : tree_builder(),                                                      //
        partitioners{QPeriodic("MPI1", 0, 2),                                //
                     Plain("Plain1", 0)},                                    //
        spd{{32, Parity::EVEN}},                                             //
        t(tree_builder(spd, partitioners)),                                  //
        max_idx_tree(get_nchildren_alloc_tree(t,                             //
                                              [](auto x) { return true; })), //
        sizetree(get_size_tree(max_idx_tree))                                //
        {};
};

struct LessSimple1D {
  PTBuilder tree_builder;
  PartList partitioners;
  SizeParityD spd;
  PartitionTree t;

  TreeP<std::pair<int, int>> max_idx_tree;
  TreeP<std::pair<int, int>> sizetree;

  LessSimple1D()
      : tree_builder(),                                                      //
        partitioners{QPeriodic("MPI1", 0, 2),                                //
                     QOpen("Vector1", 0, 2),                                 //
                     Plain("Plain1", 0)},                                    //
        spd{{32, Parity::EVEN}},                                             //
        t(tree_builder(spd, partitioners)),                                  //
        max_idx_tree(get_nchildren_alloc_tree(t,                             //
                                              [](auto x) { return true; })), //
        sizetree(get_size_tree(max_idx_tree))                                //
        {};
};

BOOST_AUTO_TEST_SUITE(test_partition_tree)

BOOST_FIXTURE_TEST_CASE(test_all_allocated_simple_maxidx, Simple1D) {

  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };

  decltype(max_idx_tree) exptree = mt(mp(2, 0), {mt(mp(16, 0), {}), //
                                                 mt(mp(16, 1), {})});
  BOOST_TEST(*max_idx_tree == *exptree);
}

BOOST_FIXTURE_TEST_CASE(test_all_allocated_simple_size, Simple1D) {

  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };

  decltype(sizetree) exptree = mt(mp(32, 0), {mt(mp(16, 0), {}), //
                                              mt(mp(16, 1), {})});
  BOOST_TEST(*sizetree == *exptree);
}
BOOST_FIXTURE_TEST_CASE(test_all_allocated_lesssimple_maxidx, LessSimple1D) {
  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };

  decltype(max_idx_tree) exptree =
      mt(mp(2, 0), {mt(mp(2, 0), {mt(mp(8, 0), {}),   //
                                  mt(mp(8, 1), {})}), //
                    mt(mp(2, 1), {mt(mp(8, 0), {}),   //
                                  mt(mp(8, 1), {})})});
  BOOST_TEST(*max_idx_tree == *exptree);
}
BOOST_FIXTURE_TEST_CASE(test_all_allocated_lesssimple_size, LessSimple1D) {
  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };

  decltype(sizetree) exptree =
      mt(mp(32, 0), {mt(mp(16, 0), {mt(mp(8, 0), {}),   //
                                    mt(mp(8, 1), {})}), //
                     mt(mp(16, 1), {mt(mp(8, 0), {}),   //
                                    mt(mp(8, 1), {})})});
  BOOST_TEST(*sizetree == *exptree);
}
BOOST_AUTO_TEST_CASE(test_all_allocated) {
  PTBuilder tree_builder;
  PartList partitioners{QPeriodic("MPIX", 0, 4),        //
                        QOpen("VectorX", 0, 2),         //
                        HBB("HalosX", 0, 1),            //
                        partitioners::EO("EO", {true}), //
                        Plain("PlainExtra", 1),         //
                        Plain("PlainX", 0)};
  SizeParityD spd{{42, Parity::EVEN}};
  PartitionTree t = tree_builder(spd, partitioners);

  TreeP<std::pair<int, int>> sizetree =
      get_size_tree(get_nchildren_alloc_tree(t, [](auto x) { return true; }));

  // halos for 4 MPI partition having 2 sub partitions
  int exp_size = 42 + 2 * 2 * 4;
  BOOST_TEST(sizetree->n.first == exp_size);
}
BOOST_AUTO_TEST_CASE(test_only_bb_allocated) {
  PTBuilder tree_builder;
  PartList partitioners{QPeriodic("MPIX", 0, 4),        //
                        QOpen("VectorX", 0, 2),         //
                        HBB("HalosX", 0, 1),            //
                        partitioners::EO("EO", {true}), //
                        Plain("PlainExtra", 1),         //
                        Plain("PlainX", 0)};
  SizeParityD spd{{42, Parity::EVEN}};
  PartitionTree t = tree_builder(spd, partitioners);

  TreeP<std::pair<int, int>> sizetree = get_size_tree(
      get_nchildren_alloc_tree(t, get_NmD_halo_predicate(partitioners, 0)));

  // no copies
  int exp_size = 42;
  BOOST_TEST(sizetree->n.first == exp_size);
}
BOOST_FIXTURE_TEST_CASE(test_4D_mpirank, Part4DF) {

  auto predicate = [&](Indices idxs) {
    vector<int> MPI_rank{3, 3, 3, 3};
    return get_mpi_rank_predicate(partitioners, //
                                  MPI_rank)(idxs) and
           get_NmD_halo_predicate(partitioners, 0)(idxs);
  };

  TreeP<std::pair<int, int>> sizetree =
      get_size_tree(get_nchildren_alloc_tree(t, predicate));
  int exp_size = 9 * 9 * 9 * 9;
  BOOST_TEST(sizetree->n.first == exp_size);
}

BOOST_FIXTURE_TEST_CASE(test_4D_mpirank_3Dhalo, Part4DF) {

  auto predicate = [&](Indices idxs) {
    vector<int> MPI_rank{3, 3, 3, 3};
    return get_mpi_rank_predicate(partitioners, //
                                  MPI_rank)(idxs) and
           get_NmD_halo_predicate(partitioners, 1)(idxs);
  };

  TreeP<std::pair<int, int>> sizetree =
      get_size_tree(get_nchildren_alloc_tree(t, predicate));
  int exp_size = 9 * 9 * 9 * 9 + // bulk volume
                 9 * 9 * 9 *     // single surface
                     4 *         // dimensions
                     2 *         // up-down
                     2;          // each direction is still split in 4+5

  BOOST_TEST(sizetree->n.first == exp_size);
}

BOOST_FIXTURE_TEST_CASE(test_offset_tree_simple, Simple1D) {

  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };
  TreeP<std::pair<int, int>> offset_tree = get_offset_tree(sizetree);
  decltype(offset_tree) expected = mt(mp(0, 0), {mt(mp(0, 0), {}), //
                                                 mt(mp(16, 1), {})});

  bool success = *offset_tree == *expected;
  BOOST_TEST(success);
}
BOOST_FIXTURE_TEST_CASE(test_offset_tree_lesssimple, LessSimple1D) {

  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };
  TreeP<std::pair<int, int>> offset_tree = get_offset_tree(sizetree);
  decltype(offset_tree) expected =
      mt(mp(0, 0), {mt(mp(0, 0), {mt(mp(0, 0), {}),   //
                                  mt(mp(8, 1), {})}), //
                    mt(mp(16, 1), {mt(mp(16, 0), {}), //
                                   mt(mp(24, 1), {})})}

      );
  BOOST_TEST(*offset_tree == *expected);
}

BOOST_AUTO_TEST_CASE(test_no_zerosize) {

  enum { X, EXTRA };
  PTBuilder tree_builder;
  auto t = tree_builder({{42, Parity::EVEN}},            //
                        {QPeriodic("MPI X", X, 4),       //
                         QOpen("VECTOR X", X, 2),        //
                         HBB("halos X", X, 1),           //
                         partitioners::EO("EO", {true}), //
                         Plain("EO-flattened", EXTRA)});

  auto size_tree =
      get_size_tree(get_nchildren_alloc_tree(t, [](auto _) { return true; }));

  for (auto si : depth_first_flatten(size_tree)) {
    int size = si.first;
    BOOST_TEST(size != 0);
  }
}

BOOST_AUTO_TEST_SUITE_END()