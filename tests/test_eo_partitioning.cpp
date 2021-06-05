#include "geometry/eo.hpp"
#include "partitioners/eo_partitioning.hpp"
#include <algorithm>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <iostream>

using namespace hypercubes::slow;
namespace bdata = boost::unit_test::data;
using partitioning::EO;

struct EO5 {

  const SizeParities sp{{11, Parity::EVEN},
                        {10, Parity::EVEN},
                        {9, Parity::ODD},
                        {5, Parity::NONE},
                        {8, Parity::ODD}};
  const typename EO::CBFlags cbflags{true, true, true, false, true};
  EO P;
  EO5() : P(sp, cbflags, "test5") {}
};

struct EOALLODD {
  const SizeParities sp{
      {11, Parity::ODD}, {11, Parity::NONE}, {11, Parity::EVEN}};
  const typename EO::CBFlags cbflags{true, false, true};
  EO P;
  EOALLODD() : P(sp, cbflags, "test_allodd") {}
};

BOOST_AUTO_TEST_SUITE(eo_partitioning)

BOOST_AUTO_TEST_CASE(test_constructor_throws1) {
  const SizeParities sp{{{11, Parity::EVEN},
                         {10, Parity::EVEN},
                         {5, Parity::NONE},
                         {9, Parity::ODD}}};
  const typename EO::CBFlags cbflags{true, true, true, false, true};

  BOOST_CHECK_THROW(EO(sp, cbflags, "test"), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_constructor_throws2) {
  const SizeParities sp{{{11, Parity::EVEN},
                         {10, Parity::EVEN},
                         {5, Parity::NONE},
                         {9, Parity::ODD}}};
  const typename EO::CBFlags cbflags{true, true, true, true};

  BOOST_CHECK_THROW(EO(sp, cbflags, "test"), std::invalid_argument);
}

BOOST_FIXTURE_TEST_CASE(test_idx_to_coords_check_dimensions, EO5) {

  Coordinates offset{2, 3, 4, 2}; // one less
  BOOST_CHECK_THROW(P.idx_to_coords(1, offset), std::invalid_argument);
}
BOOST_FIXTURE_TEST_CASE(test_idx_to_coords_len, EO5) {

  Coordinates offset{0, 0, 0, 3, 0, 23};
  auto r = P.idx_to_coords(1, offset);
  BOOST_TEST(r.size() == cbflags.size());
}

BOOST_FIXTURE_TEST_CASE(test_idx_to_coords_untouched, EO5) {

  Coordinates offset{0, 0, 0, 3, 0, 23};
  auto r = P.idx_to_coords(1, offset);
  for (int i = 0; i < cbflags.size(); ++i)
    if (not cbflags[i])
      BOOST_TEST(offset[i] == r[i]);
}

BOOST_FIXTURE_TEST_CASE(test_idx_to_coords1_23, EO5) {

  Coordinates offset{0, 0, 0, 3, 0, 23};
  auto r = P.idx_to_coords(1, offset);
  Coordinates expected{3, 4, 0, 3, 0};
  for (int i = 0; i < expected.size(); ++i)
    BOOST_TEST(r[i] == expected[i]);
}

BOOST_FIXTURE_TEST_CASE(test_idx_to_coords1_23_allodd, EOALLODD) {

  Coordinates offset{0, 3, 0, 23};
  auto r = P.idx_to_coords(1, offset);
  Coordinates expected{2, 3, 4};
  for (int i = 0; i < expected.size(); ++i)
    BOOST_TEST(r[i] == expected[i]);
}

BOOST_DATA_TEST_CASE_F(EO5, test_idx_to_coords,
                       bdata::random(0, 11 * 10 * 9 * 8 - 1) ^
                           bdata::xrange(1000),
                       idx, ignored) {

  int idxh = idx / 2;
  Parity parity = static_cast<Parity>(idx % 2);
  Coordinates offset{0, 0, 0, 3, 0, idxh};
  auto r = P.idx_to_coords(parity, offset);
  eo::IntList sizes{11, 10, 9, 8};
  Coordinates subexpected(eo::lexeo_idx_to_coord(parity, idxh, sizes));
  vector<int> subexp_index{0, 1, 2, 4};
  for (int j = 0; j < subexpected.size(); ++j) {
    int i = subexp_index[j];
    BOOST_TEST(r[i] == subexpected[j]);
  }
}

BOOST_DATA_TEST_CASE_F(EOALLODD, test_idx_to_coords_allodd,
                       bdata::xrange(11 * 11 - 1), idx) {

  int idxh = idx / 2;
  int parity = ((idx + 1) % 2);
  Parity origin_parity = static_cast<Parity>(idx % 2);
  Coordinates offset{0, 3, 0, idxh};
  auto r = P.idx_to_coords(parity, offset);
  eo::IntList sizes{11, 11};
  Coordinates subexpected(eo::lexeo_idx_to_coord(origin_parity, idxh, sizes));
  vector<int> subexp_index{0, 2};
  for (int j = 0; j < subexpected.size(); ++j) {
    int i = subexp_index[j];
    BOOST_TEST(r[i] == subexpected[j]);
  }
}

BOOST_FIXTURE_TEST_CASE(test_idx_to_sizes, EO5) {
  auto s = P.idx_to_sizes(52934572);
  BOOST_TEST(s.size() == P.dimensionality() + 1);
}

BOOST_FIXTURE_TEST_CASE(test_idx_to_child_kind_even, EO5) {
  BOOST_TEST(P.idx_to_child_kind(0) == 0);
  BOOST_TEST(P.idx_to_child_kind(1) == 0);
}

BOOST_FIXTURE_TEST_CASE(test_idx_to_child_kind_odd, EOALLODD) {
  BOOST_TEST(P.idx_to_child_kind(0) == 0);
  BOOST_TEST(P.idx_to_child_kind(1) == 1);
}

BOOST_FIXTURE_TEST_CASE(test_max_idx, EO5) {
  BOOST_TEST(P.max_idx_value() == 2);
}

BOOST_FIXTURE_TEST_CASE(test_sub_sizeparity_info_list, EO5) {
  // Even number of sites, only one class

  auto p = P.sub_sizeparity_info_list();
  decltype(p) expected{{{1, Parity::NONE},
                        {1, Parity::NONE},
                        {1, Parity::NONE},
                        {5, Parity::NONE},
                        {1, Parity::NONE},
                        {11 * 10 * 9 * 8 / 2, Parity::NONE}}};
  BOOST_TEST(p == expected);
}

BOOST_FIXTURE_TEST_CASE(test_sub_sizeparity_info_list_allodd, EOALLODD) {
  // Odd number of sites,
  // Odd partition is bigger
  // because the origin parity is odd.

  auto p = P.sub_sizeparity_info_list();
  decltype(p) expected{
      {{1, Parity::NONE},
       {11, Parity::NONE},
       {1, Parity::NONE},
       {11 * 11 / 2, Parity::NONE}},
      {{1, Parity::NONE},
       {11, Parity::NONE},
       {1, Parity::NONE},
       {11 * 11 / 2 + 1, Parity::NONE}},
  };
  BOOST_TEST(p == expected);
}

BOOST_DATA_TEST_CASE_F(EO5, test_coord_to_idxs,
                       bdata::random(0, 11 * 10 * 9 * 8 - 1) ^
                           bdata::xrange(1000),
                       idx, ignore) {
  int idxh = idx / 2;
  int parity = ((idx + 1) % 2);
  Coordinates offset{0, 0, 0, 3, 0, idxh};
  auto coords = P.idx_to_coords(parity, offset);
  auto idr = P.coord_to_idxs(coords)[0];
  BOOST_TEST(idr.idx == parity);
  BOOST_TEST(idr.rest == offset);
}

BOOST_DATA_TEST_CASE_F(EOALLODD, test_coord_to_idxs_allodd,
                       bdata::xrange(11 * 11 - 1), idx) {
  int idxh = idx / 2;
  int parity = ((idx + 1) % 2);
  Coordinates offset{0, 3, 0, idxh};
  auto coords = P.idx_to_coords(parity, offset);
  auto idr = P.coord_to_idxs(coords)[0];
  BOOST_TEST(idr.idx == parity);
  BOOST_TEST(idr.rest == offset);
}

BOOST_AUTO_TEST_SUITE_END()
