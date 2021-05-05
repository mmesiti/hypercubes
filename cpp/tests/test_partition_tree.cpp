#include "fixtures.hpp"
#include "partition_class_tree.hpp"
#include "partition_predicates.hpp"
#include "partition_tree.hpp"
#include "partitioners.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

using namespace hypercubes::slow;
using namespace hypercubes::slow::partitioners;

BOOST_AUTO_TEST_SUITE(test_partition_tree)
BOOST_AUTO_TEST_CASE(test_all_allocated_simple) {

  PCTBuilder tree_builder;
  PartList partitioners{QPeriodic("MPI1", 0, 2), //
                        Plain("Plain1", 0)};
  SizeParityD spd{{32, Parity::EVEN}};
  PartitionClassTree t = tree_builder(spd, partitioners);

  TreeP<std::pair<int, int>> sizetree =
      get_size_tree(t, [](auto x) { return true; });
  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };

  decltype(sizetree) exptree = mt(mp(32, 0), {mt(mp(16, 0), {}), //
                                              mt(mp(16, 1), {})});
  bool check = *sizetree == *exptree;
  BOOST_TEST(check);
}
BOOST_AUTO_TEST_CASE(test_all_allocated_lesssimple) {
  PCTBuilder tree_builder;
  PartList partitioners{QPeriodic("MPI1", 0, 2), //
                        QOpen("Vector1", 0, 2),  //
                        Plain("Plain1", 0)};
  SizeParityD spd{{32, Parity::EVEN}};
  PartitionClassTree t = tree_builder(spd, partitioners);

  TreeP<std::pair<int, int>> sizetree =
      get_size_tree(t, [](auto x) { return true; });
  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };

  decltype(sizetree) exptree =
      mt(mp(32, 0), {mt(mp(16, 0), {mt(mp(8, 0), {}),   //
                                    mt(mp(8, 1), {})}), //
                     mt(mp(16, 1), {mt(mp(8, 0), {}),   //
                                    mt(mp(8, 1), {})})});
  bool check = *sizetree == *exptree;
  BOOST_TEST(check);
}
BOOST_AUTO_TEST_CASE(test_all_allocated) {
  PCTBuilder tree_builder;
  PartList partitioners{QPeriodic("MPIX", 0, 4),        //
                        QOpen("VectorX", 0, 2),         //
                        HBB("HalosX", 0, 1),            //
                        partitioners::EO("EO", {true}), //
                        Plain("PlainExtra", 1),         //
                        Plain("PlainX", 0)};
  SizeParityD spd{{42, Parity::EVEN}};
  PartitionClassTree t = tree_builder(spd, partitioners);

  TreeP<std::pair<int, int>> sizetree =
      get_size_tree(t, [](auto x) { return true; });

  // halos for 4 MPI partition having 2 sub partitions
  int exp_size = 42 + 2 * 2 * 4;
  BOOST_TEST(sizetree->n.first == exp_size);
}
BOOST_AUTO_TEST_CASE(test_only_bb_allocated) {
  PCTBuilder tree_builder;
  PartList partitioners{QPeriodic("MPIX", 0, 4),        //
                        QOpen("VectorX", 0, 2),         //
                        HBB("HalosX", 0, 1),            //
                        partitioners::EO("EO", {true}), //
                        Plain("PlainExtra", 1),         //
                        Plain("PlainX", 0)};
  SizeParityD spd{{42, Parity::EVEN}};
  PartitionClassTree t = tree_builder(spd, partitioners);

  TreeP<std::pair<int, int>> sizetree =
      get_size_tree(t, get_NmD_halo_predicate(partitioners, 0));

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

  TreeP<std::pair<int, int>> sizetree = get_size_tree(t, predicate);
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

  TreeP<std::pair<int, int>> sizetree = get_size_tree(t, predicate);
  int exp_size = 9 * 9 * 9 * 9 + // bulk volume
                 9 * 9 * 9 *     // single surface
                     4 *         // dimensions
                     2 *         // up-down
                     2;          // each direction is still split in 4+5

  BOOST_TEST(sizetree->n.first == exp_size);
}
BOOST_AUTO_TEST_SUITE_END()
