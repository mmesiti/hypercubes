#include "fixtures.hpp"
#include "partition_tree.hpp"
#include "test_utils.hpp"
#include "tree.hpp"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <cmath>

using namespace hypercubes::slow;
using namespace hypercubes::slow::partitioners;
using namespace data;
namespace bdata = boost::unit_test::data;

BOOST_AUTO_TEST_SUITE(test_partition_class_tree)

BOOST_FIXTURE_TEST_CASE(test_get_indices_tree_wg_4D_full, Part4DF) {
  Coordinates xs{20, 13, 23, 4};
  // Site index:
  // |0 0000 0 0 000 1 1 1111 1 1 112 2 2 2222 2 2 233 3 3 333 3 3 34 4|
  // |0 1234 5 6 789 0 1 2345 6 7 890 1 2 3456 7 8 901 2 3 456 7 8 90 1|
  // -------------------------------------------------------------------
  // |0 0000 0 0 000 0|1 1111 1 1 111 1|2 2222 2 2 222 2|3 333 3 3 33 3|
  // |0 0000 0|1 111 1|0 0000 0|1 111 1|0 0000 0|1 111 1|0 000 0|1 11 1|
  // |1|2222|3|1|222|3|1|2222|3|1|222|3|1|2222|3|1|222|3|1|222|3|1|22|3|
  // |0|0123|0|0|012|0|0|0123|0|0|012|0|0|0123|0|0|012|0|0|012|0|0|01|0|
  //       X              X         X     X
  //      (4)            (13)      (20)  (23)
  // for xs:
  // X 20 -> 1,1,2, in-part idx: 2, in-part size: 3
  // Y 13 -> 1,0,2, in-part idx: 1, in-part size: 4
  // Z 23 -> 2,0,2, in-part idx: 0, in-part size: 4
  // T 4  -> 0,0,2, in-part idx: 3, in-part size: 4
  // Parity is: 0
  // IDX in the eo partition is
  //(2 + 3*1 + 3*4*0 + 3*4*4*3) // 2 = 149 // 2 = 74
  // So, the expected index is
  vector<int> expected_idx{1, 1, 2, 0, 1, 0, 0, 0, 2, 2, 2, 2, 0, 74};
  //                      |----------------------------------| |  |
  //                                 transpose of              |  |
  //                               XYZT idxs/sub-idxs          |  |
  //                                                           |  |
  //                                                      parity  |
  //                                                       in-eo idx
  //                                                       Coordinates xs{20,
  //                                                       13, 23, 4};

  Indices idxs = get_real_indices(t, xs);

  BOOST_TEST(idxs == expected_idx);
}

BOOST_FIXTURE_TEST_CASE(test_ghosts, Part4DF) {

  auto datasource = data::rilist(0, 41, 4, 4);
  auto xs = datasource.begin();
  for (int icase = 0; icase < 500; ++icase) {
    auto count = [](auto site_list, auto xs) {
      int c = 0;
      for (auto x : xs)
        if (std::find(site_list.begin(), site_list.end(), x) != site_list.end())
          ++c;
      return c;
    };

    int nbulk = count(bulk_sites, *xs);
    int nborder = count(border_sites, *xs);

    BOOST_TEST(nbulk + nborder == (*xs).size());
    auto itwg = get_indices_tree_with_ghosts(t, *xs);
    auto idxs = get_relevant_indices_flat(itwg);
    BOOST_TEST(idxs.size() == pow(2, nborder));
    int nghosts = std::count_if(idxs.begin(),                  //
                                idxs.end(),                    //
                                [](auto idx) {                 //
                                  return std::get<0>(idx) > 0; //
                                });
    BOOST_TEST(nghosts == idxs.size() - 1);
    ++xs;
  }
}

BOOST_FIXTURE_TEST_CASE(test_get_coords_from_idx_roundtrip, Part4DF) {

  auto datasource = data::rilist(0, 41, 4, 4);
  auto xs = datasource.begin();
  for (int icase = 0; icase < 500; ++icase) {

    Indices idx = get_real_indices(t, *xs);
    Coordinates new_coords = get_coord_from_idx(t, idx);
    BOOST_TEST(new_coords == *xs);
    ++xs;
  }
}

BOOST_DATA_TEST_CASE_F(Part4DFWLocal, test_coord_idx_roundtrip_wlocal,
                       bdata::xrange(8), loc) {

  auto datasource = data::rilist(0, 41, 4, 4);
  auto xs = datasource.begin();
  for (int icase = 0; icase < 10; ++icase) {
    Coordinates xs_wloc = *xs;
    xs_wloc.push_back(loc);
    Indices idx = get_real_indices(t, xs_wloc);
    Coordinates new_coords = get_coord_from_idx(t, idx);
    BOOST_TEST(new_coords == xs_wloc);
    ++xs;
  }
}
BOOST_DATA_TEST_CASE_F(Part1D42, test_validate_idx_yes, //
                       bdata::make(vector<vector<int>>{{3, 1, 0, 1, 0},
                                                       {0, 0, 4, 0, 0},
                                                       {2, 1, 1, 0, 0},
                                                       {1, 0, 2, 1, 0},
                                                       {0, 1, 2, 1, 1}}),
                       idx) {

  BOOST_TEST(validate_idx(t, idx));
}
BOOST_DATA_TEST_CASE_F(Part1D42, test_validate_idx_no,                   //
                       bdata::make(vector<vector<int>>{{3, 1, 0, 1, 1},  //
                                                       {0, 0, 4, 0, -1}, //
                                                       {0, 0, 5, 0, 0},  //
                                                       {4, 1, 0, 1, 0},  //
                                                       {5, -1, 0, 1, 0}, //
                                                       {5, 1, 0, 1, 0},  //
                                                       {3, 0, 3, 0, 0},  //
                                                       {2, 1, 2, 0, 1},  //
                                                       {1, 0, 1, 0, 0}}),
                       idx) {

  BOOST_TEST(not validate_idx(t, idx));
}

BOOST_AUTO_TEST_SUITE_END()
