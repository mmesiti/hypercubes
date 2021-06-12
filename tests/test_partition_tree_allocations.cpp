#include "fixtures.hpp"
#include "partitioners/partitioners.hpp"
#include "selectors/partition_predicates.hpp"
#include "selectors/prune_tree.hpp"
#include "test_utils.hpp"
#include "trees/kvtree.hpp"
#include "trees/partition_tree.hpp"
#include "trees/partition_tree_allocations.hpp"
#include "trees/tree.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

using namespace hypercubes::slow;
using namespace hypercubes::slow::internals;
using namespace hypercubes::slow::partitioners;

namespace pm = partitioner_makers;

struct Simple1D {
  PartList partitioners;
  SizeParityD spd;
  PartitionTree t;
  TreeP<std::pair<int, int>> nalloc_children_tree;
  TreeP<std::pair<int, int>> sizetree;

  Simple1D()
      : partitioners{pm::QPeriodic("MPI1", 0, 2),                          //
                     pm::Plain("Plain1", 0),                               //
                     pm::Site()},                                          //
        spd{{32, Parity::EVEN}},                                           //
        t(get_partition_tree(spd, partitioners)),                          //
        nalloc_children_tree(prune_tree(get_nchildren_alloc_tree(t),       //
                                        [](auto x) { return BoolM::T; })), //
        sizetree(get_size_tree(nalloc_children_tree))                      //
        {};
};

struct LessSimple1D {
  PartList partitioners;
  SizeParityD spd;
  PartitionTree t;

  TreeP<std::pair<int, int>> nalloc_children_tree;
  TreeP<std::pair<int, int>> sizetree;

  LessSimple1D()
      :
        partitioners{
            pm::QPeriodic("MPI1", 0, 2), //
            pm::QOpen("Vector1", 0, 2),  //
            pm::Plain("Plain1", 0),      //
            pm::Site(),
        },                                  //
        spd{{32, Parity::EVEN}},            //
        t(get_partition_tree(spd, partitioners)), //
        nalloc_children_tree(
            prune_tree(get_nchildren_alloc_tree(t),                             //
                                     [](auto x) { return BoolM::T; })), //
        sizetree(get_size_tree(nalloc_children_tree))               //
        {};
};

BOOST_AUTO_TEST_SUITE(test_partition_tree)

BOOST_FIXTURE_TEST_CASE(test_all_allocated_simple_maxidx, Simple1D) {

  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };

  decltype(nalloc_children_tree) exptree = mt(mp(0, 2), {mt(mp(0, 16), {}), //
                                                         mt(mp(1, 16), {})});
  int site_level = 2;
  BOOST_TEST(*truncate_tree(nalloc_children_tree, site_level) == *exptree);
}

BOOST_FIXTURE_TEST_CASE(test_all_allocated_simple_size, Simple1D) {

  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };

  decltype(sizetree) exptree = mt(mp(0, 32), {mt(mp(0, 16), {}), //
                                              mt(mp(1, 16), {})});
  int site_level = 2;
  BOOST_TEST(*truncate_tree(sizetree, site_level) == *exptree);
}
BOOST_FIXTURE_TEST_CASE(test_all_allocated_lesssimple_maxidx, LessSimple1D) {
  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };

  decltype(nalloc_children_tree) exptree =
      mt(mp(0, 2), {mt(mp(0, 2), {mt(mp(0, 8), {}),   //
                                  mt(mp(1, 8), {})}), //
                    mt(mp(1, 2), {mt(mp(0, 8), {}),   //
                                  mt(mp(1, 8), {})})});
  int site_level = 3;
  BOOST_TEST(*truncate_tree(nalloc_children_tree, site_level) == *exptree);
}
BOOST_FIXTURE_TEST_CASE(test_all_allocated_lesssimple_size, LessSimple1D) {
  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };

  decltype(sizetree) exptree =
      mt(mp(0, 32), {mt(mp(0, 16), {mt(mp(0, 8), {}),   //
                                    mt(mp(1, 8), {})}), //
                     mt(mp(1, 16), {mt(mp(0, 8), {}),   //
                                    mt(mp(1, 8), {})})});
  int site_level = 3;
  BOOST_TEST(*truncate_tree(sizetree, site_level) == *exptree);
}
BOOST_AUTO_TEST_CASE(test_all_allocated) {
  PartList partitioners{pm::QPeriodic("MPIX", 0, 4), //
                        pm::QOpen("VectorX", 0, 2),  //
                        pm::HBB("HalosX", 0, 1),     //
                        pm::EO("EO", {true}),        //
                        pm::Plain("PlainExtra", 1),  //
                        pm::Plain("PlainX", 0),      //
                        pm::Site()};
  SizeParityD spd{{42, Parity::EVEN}};
  PartitionTree t = get_partition_tree(spd, partitioners);

  TreeP<std::pair<int, int>> sizetree = get_size_tree(
      prune_tree(get_nchildren_alloc_tree(t), [](auto x) { return BoolM::T; }));

  // halos for 4 MPI partition having 2 sub partitions
  int exp_size = 42 + 2 * 2 * 4;
  BOOST_TEST(sizetree->n.second == exp_size);
}
BOOST_AUTO_TEST_CASE(test_only_bb_allocated) {
  PartList partitioners{pm::QPeriodic("MPIX", 0, 4), //
                        pm::QOpen("VectorX", 0, 2),  //
                        pm::HBB("HalosX", 0, 1),     //
                        pm::EO("EO", {true}),        //
                        pm::Plain("PlainExtra", 1),  //
                        pm::Plain("PlainX", 0),      //
                        pm::Site()};
  SizeParityD spd{{42, Parity::EVEN}};
  PartitionTree t = get_partition_tree(spd, partitioners);

  auto nct = prune_tree(get_nchildren_alloc_tree(t),
                        getp(halos_up_to_NmD, partitioners, 0));
  TreeP<std::pair<int, int>> sizetree = get_size_tree(nct);

  // no copies
  int exp_size = 42;
  BOOST_TEST(sizetree->n.second == exp_size);
}
BOOST_FIXTURE_TEST_CASE(test_4D_mpirank, Part4DF) {

  auto predicate = [&](Indices idxs) {
    vector<int> MPI_rank{3, 3, 3, 3};
    return getp(mpi_rank,partitioners, //
                                  MPI_rank)(idxs) and
           getp(halos_up_to_NmD, partitioners, 0)(idxs);
  };

  TreeP<std::pair<int, int>> sizetree =
      get_size_tree(prune_tree(get_nchildren_alloc_tree(t), predicate));
  int exp_size = 9 * 9 * 9 * 9;
  BOOST_TEST(sizetree->n.second == exp_size);
}

BOOST_FIXTURE_TEST_CASE(test_4D_mpirank_3Dhalo, Part4DF) {

  auto predicate = [&](Indices idxs) {
    vector<int> MPI_rank{3, 3, 3, 3};
    return getp(mpi_rank,partitioners, //
                                  MPI_rank)(idxs) and
           getp(halos_up_to_NmD, partitioners, 1)(idxs);
  };

  TreeP<std::pair<int, int>> sizetree =
      get_size_tree(prune_tree(get_nchildren_alloc_tree(t), predicate));
  int exp_size = 9 * 9 * 9 * 9 + // bulk volume
                 9 * 9 * 9 *     // single surface
                     4 *         // dimensions
                     2 *         // up-down
                     2;          // each direction is still split in 4+5

  BOOST_TEST(sizetree->n.second == exp_size);
}

BOOST_FIXTURE_TEST_CASE(test_offset_tree_simple, Simple1D) {

  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };
  TreeP<std::pair<int, int>> offset_tree = get_offset_tree(sizetree);
  decltype(offset_tree) expected = mt(mp(0, 0), {mt(mp(0, 0), {}), //
                                                 mt(mp(1, 16), {})});

  int site_level = 2;
  BOOST_TEST(*truncate_tree(offset_tree, site_level) == *expected);
}
BOOST_FIXTURE_TEST_CASE(test_offset_tree_lesssimple, LessSimple1D) {

  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };
  TreeP<std::pair<int, int>> offset_tree = get_offset_tree(sizetree);
  decltype(offset_tree) expected =
      mt(mp(0, 0), {mt(mp(0, 0), {mt(mp(0, 0), {}),   //
                                  mt(mp(1, 8), {})}), //
                    mt(mp(1, 16), {mt(mp(0, 16), {}), //
                                   mt(mp(1, 24), {})})}

      );
  int site_level = 3;
  BOOST_TEST(*truncate_tree(offset_tree, site_level) == *expected);
}

BOOST_AUTO_TEST_CASE(test_no_zerosize) {

  enum { X, EXTRA };
  auto t = get_partition_tree({{42, Parity::EVEN}}, //
                              {
                                  pm::QPeriodic("MPI X", X, 4),     //
                                  pm::QOpen("VECTOR X", X, 2),      //
                                  pm::HBB("halos X", X, 1),         //
                                  pm::EO("EO", {true}),             //
                                  pm::Plain("EO-flattened", EXTRA), //
                                  pm::Site(),                       //
                              });

  auto size_tree = get_size_tree(
      prune_tree(get_nchildren_alloc_tree(t), [](auto _) { return BoolM::T; }));

  for (auto si : depth_first_flatten(size_tree)) {
    int size = si.second;
    BOOST_TEST(size != 0);
  }
}

BOOST_AUTO_TEST_SUITE_END()
