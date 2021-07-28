
#include "fixtures.hpp"
#include "selectors/partition_predicates.hpp"
#include "selectors/prune_tree.hpp"
#include "trees/partition_tree_allocations.hpp"
#include <boost/test/unit_test.hpp>

using namespace hypercubes::slow;
using namespace hypercubes::slow::internals;
BOOST_AUTO_TEST_SUITE(test_prune_tree)

BOOST_AUTO_TEST_CASE(test_prune_tree_simple) {

  auto tfull =
      mt(mp(0, 1), {mt(mp(0, 2), {mt(mp(0, 3), {mt(mp(0, 5), {}),     //
                                                mt(mp(1, 6), {})})}), //
                    mt(mp(1, 4), {mt(mp(10, 3), {mt(mp(0, 7), {}),    //
                                                 mt(mp(1, 8), {})})})});

  auto predicate = [](hypercubes::slow::Indices idxs) {
    using hypercubes::slow::BoolM;
    if (idxs.size() > 0) {
      if (idxs[0] != 1)
        return BoolM::F;
      else
        return BoolM::T;
    }

    return BoolM::M;
  };
  auto expt = mt(mp(0, 1), {mt(mp(1, 4), {mt(mp(10, 3), {mt(mp(0, 7), {}), //
                                                         mt(mp(1, 8), {})})})});

  auto t = prune_tree(tfull, predicate);
  BOOST_TEST(*t == *expt);
}

BOOST_FIXTURE_TEST_CASE(test_and, Part1D42) {

  PartitionPredicate mpi_selector01 =
      getp(selectors::mpi_rank, partitioners, {0}) or
      getp(selectors::mpi_rank, partitioners, {1});

  PartitionPredicate bb_selector =
      getp(selectors::halos_upto_NmD, partitioners, 0);

  auto nchildren_tree = get_skeleton_tree(t);
  auto nopruned = prune_tree(nchildren_tree, bb_selector);
  BOOST_TEST(*nchildren_tree != *nopruned);

  auto pruned_two_steps =
      prune_tree(prune_tree(nchildren_tree, mpi_selector01), bb_selector);

  auto pruned_composite =
      prune_tree(nchildren_tree,
                 bb_selector and mpi_selector01); // and border_selector);

  BOOST_TEST(*pruned_composite == *pruned_two_steps);
}
BOOST_FIXTURE_TEST_CASE(test_commute, Part1D42) {

  PartitionPredicate mpi_selector01 =
      getp(selectors::mpi_rank, partitioners, {0}) or
      getp(selectors::mpi_rank, partitioners, {1});

  PartitionPredicate bb_selector =
      getp(selectors::halos_upto_NmD, partitioners, 0);

  auto nchildren_tree = get_skeleton_tree(t);
  auto nopruned = prune_tree(nchildren_tree, bb_selector);
  BOOST_TEST(*nchildren_tree != *nopruned);

  auto pruned_ab =
      prune_tree(prune_tree(nchildren_tree, mpi_selector01), bb_selector);
  auto pruned_ba =
      prune_tree(prune_tree(nchildren_tree, bb_selector), mpi_selector01);

  BOOST_TEST(*pruned_ab == *pruned_ba);
}
BOOST_AUTO_TEST_SUITE_END()
