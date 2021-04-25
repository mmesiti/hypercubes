#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include "1D/q1D.hpp"
#include "dimensionalise.hpp"

using namespace hypercubes::slow;
namespace bdata = boost::unit_test::data;

template <class Q1Dtype> struct QF {

  // Multi-dimensional
  const Dimensionalise<Q1Dtype> partitioning;
  // Mono-dimensional
  const Q1Dtype partitioning1D;
  // const int dimension = 1;
  // const int nparts = 4;
  QF()
      : partitioning(
            SizeParities{{42, Parity::EVEN}, {42, Parity::EVEN}}, /* sp */
            1,                                                    /*dimension*/
            "test2D",                                             /*name*/
            4 /*nparts*/),
        partitioning1D(SizeParity{42, Parity::EVEN}, /* sp */
                       1,                            /*dimension*/
                       "test1D",                     /*name*/
                       4 /*nparts*/){};
};

// regression between monodimensional and dimensionalized
// the periodic/open versions might be redundant.
// maybe we could do for each 1D class using boost::mpl?
BOOST_AUTO_TEST_SUITE(test_Q)

auto test_idx_to_coords_helper = [](auto p, auto p1D, int in) {
  int idx = in / 11, offset = in % 11;

  Coordinates offsets{5, in % 11};
  int i = p.idx_to_coords(idx, offsets)[p.dimension];
  int i1D = p1D.idx_to_coord(idx, offset);
  return std::make_tuple(i, i1D);
};

BOOST_DATA_TEST_CASE_F(QF<Q1DPeriodic>,             //
                       test_idx_to_coords_periodic, //
                       bdata::xrange(42), in) {

  int i, i1D;
  std::tie(i, i1D) =
      test_idx_to_coords_helper(partitioning, partitioning1D, in);

  BOOST_TEST(i == i1D);
}

BOOST_DATA_TEST_CASE_F(QF<Q1DOpen>,             //
                       test_idx_to_coords_open, //
                       bdata::xrange(42), in) {
  int i, i1D;
  std::tie(i, i1D) =
      test_idx_to_coords_helper(partitioning, partitioning1D, in);
  BOOST_TEST(i == i1D);
}

auto test_idx_to_sizes_helper = [](auto p, auto p1D, int in) {
  int idx = in / 11, offset = in % 11;

  Sizes sizes{5, in % 11};
  int s = p.idx_to_sizes(idx, sizes)[p.dimension];
  int s1D = p1D.idx_to_size(idx);
  return std::make_tuple(s, s1D);
};

BOOST_DATA_TEST_CASE_F(QF<Q1DPeriodic>,            //
                       test_idx_to_sizes_periodic, //
                       bdata::xrange(42), in) {

  int s, s1D;
  std::tie(s, s1D) = test_idx_to_sizes_helper(partitioning, partitioning1D, in);

  BOOST_TEST(s == s1D);
}

BOOST_DATA_TEST_CASE_F(QF<Q1DOpen>,            //
                       test_idx_to_sizes_open, //
                       bdata::xrange(42), in) {

  int s, s1D;
  std::tie(s, s1D) = test_idx_to_sizes_helper(partitioning, partitioning1D, in);

  BOOST_TEST(s == s1D);
}

template <class Q1D>
auto test_sub_sizeparity_info_list_helper(Dimensionalise<Q1D> p, Q1D p1D) {
  SizeParitiesD spss = p.sub_sizeparity_info_list();
  SizeParities sp1D = p1D.sub_sizeparity_info_list();
  SizeParities sp;

  std::transform(spss.begin(), spss.end(), std::back_inserter(sp),
                 [&](vector<SizeParity> sps) { return sps[p.dimension]; });

  return std::make_tuple(sp, sp1D);
};

BOOST_FIXTURE_TEST_CASE(test_sub_sizeparity_periodic, QF<Q1DPeriodic>) {

  SizeParities sp, sp1D;
  std::tie(sp, sp1D) =
      test_sub_sizeparity_info_list_helper(partitioning, partitioning1D);

  BOOST_TEST(sp == sp1D);
}

BOOST_FIXTURE_TEST_CASE(test_sub_sizeparity_open, QF<Q1DOpen>) {

  SizeParities sp, sp1D;
  std::tie(sp, sp1D) =
      test_sub_sizeparity_info_list_helper(partitioning, partitioning1D);

  BOOST_TEST(sp == sp1D);
}

auto test_idx_to_child_kind_helper = [](auto p, auto p1D, int idx) {
  int child_kind = p.idx_to_child_kind(idx);
  int child_kind1D = p1D.idx_to_child_kind(idx);
  return std::make_tuple(child_kind, child_kind1D);
};

BOOST_DATA_TEST_CASE_F(QF<Q1DPeriodic>,                 //
                       test_idx_to_child_kind_periodic, //
                       bdata::xrange(4), idx) {

  int ck, ck1D;
  std::tie(ck, ck1D) =
      test_idx_to_child_kind_helper(partitioning, partitioning1D, idx);

  BOOST_TEST(ck == ck1D);
}

BOOST_DATA_TEST_CASE_F(QF<Q1DOpen>,                 //
                       test_idx_to_child_kind_open, //
                       bdata::xrange(4), idx) {

  int ck, ck1D;
  std::tie(ck, ck1D) =
      test_idx_to_child_kind_helper(partitioning, partitioning1D, idx);

  BOOST_TEST(ck == ck1D);
}

BOOST_FIXTURE_TEST_CASE(test_max_idx_periodic, QF<Q1DPeriodic>) {

  BOOST_TEST(partitioning.max_idx_value() == partitioning1D.max_idx_value());
}

auto test_coord_to_idx_helper = [](auto partitioning, auto partitioning1D,
                                   int i, int j) {
  Coordinates ij{i, j};
  vector<IndexResultD> r = partitioning.coord_to_idxs(ij);
  vector<IndexResult> r1D_ = partitioning1D.coord_to_idxs(j);
  decltype(r1D_) r1D;
  std::transform(r.begin(), r.end(), std::back_inserter(r1D),
                 [&partitioning](IndexResultD ir) {
                   return IndexResult{ir.idx, ir.rest[partitioning.dimension],
                                      ir.cached_flag};
                 });
  return std::make_tuple(r1D_, r1D);
};

BOOST_DATA_TEST_CASE_F(QF<Q1DPeriodic>,            //
                       test_coord_to_idx_periodic, //
                       bdata::xrange(-1, 43) * bdata::xrange(-1, 43), i, j) {

  auto helperR = test_coord_to_idx_helper(partitioning, partitioning1D, i, j);
  auto r1D_ = std::get<0>(helperR);
  auto r1D = std::get<1>(helperR);
  BOOST_TEST(r1D_ == r1D);
}

BOOST_DATA_TEST_CASE_F(QF<Q1DOpen>,            //
                       test_coord_to_idx_open, //
                       bdata::xrange(-1, 43) * bdata::xrange(-1, 43), i, j) {
  auto helperR = test_coord_to_idx_helper(partitioning, partitioning1D, i, j);
  auto r1D_ = std::get<0>(helperR);
  auto r1D = std::get<1>(helperR);
  BOOST_TEST(r1D_ == r1D);
}

BOOST_AUTO_TEST_SUITE_END()
