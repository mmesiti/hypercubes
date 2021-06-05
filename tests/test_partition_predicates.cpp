#include "fixtures.hpp"
#include "selectors/bool_maybe.hpp"
#include "selectors/partition_predicates.hpp"
#include "test_utils.hpp"
#include "trees/partition_tree.hpp"
#include <algorithm>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

using namespace hypercubes::slow;
using namespace hypercubes::slow::internals;
using namespace hypercubes::slow::partitioners;
using namespace data;
namespace bdata = boost::unit_test::data;

BOOST_AUTO_TEST_SUITE(test_predicates)

BOOST_FIXTURE_TEST_CASE(test_only_bulk_and_borders, Part4DF) {
  auto data = rilist(0, 4, 4, 4);
  auto idhbb_it = data.begin();
  for (int _ = 0; _ < 200; ++_) {
    auto idhbb = *idhbb_it;
    ++idhbb_it;

    bool expected = std::all_of(idhbb.begin(), idhbb.end(),
                                [](int i) { return i != 0 and i != 4; });

    std::vector<int> idx{2,  //
                         3,  //
                         1,  //
                         0,  //# End of MPI
                         1,  //
                         2,  //
                         1,  //
                         1}; //# End of Vector

    std::copy(idhbb.begin(), idhbb.end(), std::back_inserter(idx));
    idx.push_back(0);
    idx.push_back(0);

    BOOST_CHECK(to_bool(get_NmD_halo_predicate(partitioners, 0)(idx)) ==
                expected);
  }
}

BOOST_FIXTURE_TEST_CASE(test_onlyNmD_halos_T, Part1D42) {
  BOOST_TEST(only_NmD_halos(partitioners, Indices{0, 0, 1, 0, 0}, 0) ==
             BoolM::T);
}

BOOST_FIXTURE_TEST_CASE(test_mpi_selection_wrong, Part4DF) {
  auto data = rilist(0, 3, 4, 4);
  auto data_it = data.begin();
  for (int _ = 0; _ < 1000; ++_) {
    auto MPI_rank_idx = *data_it;
    ++data_it;
    auto MPI_rank_selected = *data_it;
    ++data_it;
    if (MPI_rank_idx == MPI_rank_selected)
      continue;

    Indices idx{-1, //
                -1, //
                -1, //
                -1, //# End of MPI
                1,  //
                2,  //
                1,  //
                1,  //# End of Vector
                1,  //
                2,  //
                0,  //
                3,  // # End of halos
                0,  //
                0};

    std::copy(MPI_rank_idx.begin(), //
              MPI_rank_idx.end(),   //
              idx.begin());

    auto predicate = get_mpi_rank_predicate(partitioners, MPI_rank_selected);
    BOOST_CHECK(predicate(idx) == BoolM::F);
  }
}
BOOST_FIXTURE_TEST_CASE(test_mpi_selection_right, Part4DF) {
  auto data = rilist(0, 3, 4, 4);
  auto data_it = data.begin();
  for (int _ = 0; _ < 1000; ++_) {
    auto MPI_rank_idx = *data_it;
    ++data_it;

    Indices idx{-1, //
                -1, //
                -1, //
                -1, //# End of MPI
                1,  //
                2,  //
                1,  //
                1,  //# End of Vector
                1,  //
                2,  //
                0,  //
                3,  // # End of halos
                0,  //
                0};

    std::copy(MPI_rank_idx.begin(), //
              MPI_rank_idx.end(),   //
              idx.begin());

    auto predicate = get_mpi_rank_predicate(partitioners, MPI_rank_idx);
    BOOST_TEST(predicate(idx) == BoolM::T);
  }
}

BOOST_FIXTURE_TEST_CASE(test_hbb_slice, Part4DF) {
  auto data = rilist(0, 4, 4, 4);
  auto data_it = data.begin();
  for (int xx = 0; xx < 1000; ++xx) {
    int dir = xx % 4;
    int slice = xx / 4;
    auto HBB_idxs = *data_it;
    ++data_it;

    Indices idx{
        0,  //
        1,  //
        1,  //
        0,  //# End of MPI
        1,  //
        2,  //
        1,  //
        1,  //# End of Vector
        -1, //
        -1, //
        -1, //
        -1, // # End of halos
    };

    std::copy(HBB_idxs.begin(), //
              HBB_idxs.end(),   //
              idx.begin() + 8);

    auto predicate = get_hbb_slice_predicate(partitioners, dir, slice);
    int dir_index = 8 + dir;
    BOOST_TEST(to_bool(predicate(idx)) == (idx[dir_index] == slice));
  }
}

BOOST_DATA_TEST_CASE_F(Part1D42, test_no_border_bulk_no, bdata::xrange(42), i) {

  Indices idx = get_real_indices(t, Coordinates{i});
  BOOST_TEST(no_bulk_borders(partitioners, idx) == BoolM::F);
}

BOOST_AUTO_TEST_SUITE_END()
