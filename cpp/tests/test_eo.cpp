#define BOOST_TEST_MODULE test_eo
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>

#include "eo.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cstdlib>
#include <iostream>

using namespace hypercubes::slow;
namespace bdata = boost::unit_test::data;
BOOST_AUTO_TEST_SUITE(test_eo)

const bool debug = false;

BOOST_AUTO_TEST_CASE(test_lex_idx_to_coord) {

  std::vector<int> sizes{3, 3, 3};
  std::vector<int> exp_coord{1, 2, 2};
  int i = 1 + 2 * 3 + 2 * 3 * 3;
  auto coord = eo::lex_idx_to_coord(i, sizes);
  BOOST_TEST(coord == exp_coord);

  if (debug) { // constexpr
    std::cout << "exp_coord: " << exp_coord;
    std::cout << "coord: " << coord;
  }
}

BOOST_DATA_TEST_CASE(test_eo_idx_conversion_pair,
                     data::rilist(1, 10, 1, 6) ^ bdata::xrange(1000), sizes,
                     count) {
  // i -> coords -> eo/idxh -> coords
  const auto cumsizes = eo::get_cumsizes(sizes);
  const int isite_max = *cumsizes.rbegin();
  const int i = std::rand() % isite_max;

  auto coords = eo::lex_idx_to_coord(i, sizes);

  Parity eoflag;
  int idxh;
  std::tie(eoflag, idxh) = eo::lex_coord_to_eoidx(coords, cumsizes);

  auto coords2 = eo::lexeo_idx_to_coord(eoflag, idxh, sizes);

  bool success = coords2 == coords;
  if (not success) {
    std::cout << "cumsizes: " << cumsizes;
    std::cout << "i: " << i << std::endl;
    std::cout << "coords: " << coords;
    std::cout << "eoflags,idxh: " << eoflag << ", " << idxh << std::endl;
    std::cout << "coords2: " << coords2;
  }
  BOOST_TEST(success);
}

BOOST_DATA_TEST_CASE(test_eo_idx_conversion_pair2,
                     data::rilist(1, 10, 1, 6) ^ bdata::xrange(1000), sizes,
                     count) {
  // coords -> eo/idxh -> coords
  const auto cumsizes = eo::get_cumsizes(sizes);
  std::vector<int> coords;
  std::transform(sizes.begin(),              //
                 sizes.end(),                //
                 std::back_inserter(coords), //
                 [](int s) { return std::rand() % s; });

  Parity eoflag;
  int idxh;
  std::tie(eoflag, idxh) = eo::lex_coord_to_eoidx(coords, cumsizes);

  auto coords2 = eo::lexeo_idx_to_coord(eoflag, idxh, sizes);
  bool success = coords2 == coords;
  if (not success) {
    std::cout << "cumsizes: " << cumsizes;
    std::cout << "coords: " << coords;
    std::cout << "eoflags,idxh: " << eoflag << ", " << idxh << std::endl;
    std::cout << "coords2: " << coords2;
  }
  BOOST_TEST(success);
}

BOOST_AUTO_TEST_SUITE_END()
