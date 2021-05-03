#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include "1D/hbb1D.hpp"
#include "1D/plain1D.hpp"
#include "1D/q1D.hpp"
#include "dimensionalise.hpp"

using namespace hypercubes::slow;
namespace bdata = boost::unit_test::data;

#define QSIZE 42
#define HSIZE 42

template <class Q1Dtype> struct QF {

  // Multi-dimensional
  const Dimensionalise<Q1Dtype> partitioning;
  // Mono-dimensional
  const Q1Dtype partitioning1D;
  // const int dimension = 1;
  // const int nparts = 4;
  QF()
      : partitioning(
            SizeParities{{QSIZE, Parity::EVEN}, {QSIZE, Parity::EVEN}}, /* sp */
            1,        /*dimension*/
            "test2D", /*name*/
            4 /*nparts*/),
        partitioning1D(SizeParity{QSIZE, Parity::EVEN}, /* sp */
                       1,                               /*dimension*/
                       "test1D",                        /*name*/
                       4 /*nparts*/){};
};

struct HBBF {

  // Multi-dimensional
  const Dimensionalise<HBB1D> partitioning;
  // Mono-dimensional
  const HBB1D partitioning1D;
  // const int dimension = 1;
  // const int nparts = 4;
  HBBF()
      : partitioning(
            SizeParities{{HSIZE, Parity::EVEN}, {HSIZE, Parity::EVEN}}, /* sp */
            1,        /*dimension*/
            "test2D", /*name*/
            1 /*halo*/),
        partitioning1D(SizeParity{HSIZE, Parity::EVEN}, /* sp */
                       1,                               /*dimension*/
                       "test1D",                        /*name*/
                       1 /*halo*/){};
};

struct PlainF {

  // Multi-dimensional
  const Dimensionalise<Plain1D> partitioning;
  // Mono-dimensional
  const Plain1D partitioning1D;
  // const int dimension = 1;
  // const int nparts = 4;
  PlainF()
      : partitioning(
            SizeParities{{HSIZE, Parity::EVEN}, {HSIZE, Parity::EVEN}}, /* sp */
            1,                                          /*dimension*/
            "test2D"),                                  /*name*/
        partitioning1D(SizeParity{HSIZE, Parity::EVEN}, /* sp */
                       1,                               /*dimension*/
                       "test1D"){};                     /*name*/
};

// regression between monodimensional and dimensionalized
// the periodic/open versions might be redundant.
// maybe we could do for each 1D class using boost::mpl?
BOOST_AUTO_TEST_SUITE(test_Q)

/*****************
 * idx_to_coords *
 *****************/
auto get_idx_real = [](auto p1D, int in) {
  auto idx_results = p1D.coord_to_idxs(in);
  return *std::find_if(idx_results.begin(), idx_results.end(),
                       [](auto ir) { return not ir.cached_flag; });
};

auto test_idx_to_coords_helper = [](auto p, auto p1D, int in) {
  auto idx_real = get_idx_real(p1D, in);
  Coordinates offsets{5, idx_real.rest};
  int i = p.idx_to_coords(idx_real.idx, offsets)[p.dimension];
  int i1D = p1D.idx_to_coord(idx_real.idx, idx_real.rest);
  BOOST_TEST(i == i1D);
};

BOOST_DATA_TEST_CASE_F(QF<Q1DPeriodic>,             //
                       test_idx_to_coords_periodic, //
                       bdata::xrange(QSIZE), in) {
  test_idx_to_coords_helper(partitioning, partitioning1D, in);
}

BOOST_DATA_TEST_CASE_F(QF<Q1DOpen>,             //
                       test_idx_to_coords_open, //
                       bdata::xrange(QSIZE), in) {
  test_idx_to_coords_helper(partitioning, partitioning1D, in);
}

BOOST_DATA_TEST_CASE_F(HBBF,                   //
                       test_idx_to_coords_hbb, //
                       bdata::xrange(HSIZE), in) {
  test_idx_to_coords_helper(partitioning, partitioning1D, in);
}

BOOST_DATA_TEST_CASE_F(PlainF,                   //
                       test_idx_to_coords_plain, //
                       bdata::xrange(HSIZE), in) {
  test_idx_to_coords_helper(partitioning, partitioning1D, in);
}

/****************
 * idx_to_sizes *
 ****************/
auto test_idx_to_sizes_helper = [](auto p, auto p1D, int in) {
  auto idx_real = get_idx_real(p1D, in);

  Sizes sizes{2, 0};
  int s = p.idx_to_sizes(idx_real.idx, sizes)[p.dimension];
  int s1D = p1D.idx_to_size(idx_real.idx);
  BOOST_TEST(s == s1D);
};

BOOST_DATA_TEST_CASE_F(QF<Q1DPeriodic>,            //
                       test_idx_to_sizes_periodic, //
                       bdata::xrange(QSIZE), in) {
  test_idx_to_sizes_helper(partitioning, partitioning1D, in);
}

BOOST_DATA_TEST_CASE_F(QF<Q1DOpen>,            //
                       test_idx_to_sizes_open, //
                       bdata::xrange(QSIZE), in) {
  test_idx_to_sizes_helper(partitioning, partitioning1D, in);
}

BOOST_DATA_TEST_CASE_F(HBBF,                  //
                       test_idx_to_sizes_hbb, //
                       bdata::xrange(HSIZE), in) {
  test_idx_to_sizes_helper(partitioning, partitioning1D, in);
}

BOOST_DATA_TEST_CASE_F(PlainF,                  //
                       test_idx_to_sizes_plain, //
                       bdata::xrange(HSIZE), in) {
  test_idx_to_sizes_helper(partitioning, partitioning1D, in);
}

/****************************
 * sub_sizeparity_info_list *
 ****************************/

template <class P1D>
auto test_sub_sizeparity_info_list_helper(Dimensionalise<P1D> p, P1D p1D) {
  SizeParitiesD spss = p.sub_sizeparity_info_list();
  SizeParities sp1D = p1D.sub_sizeparity_info_list();
  SizeParities sp;

  std::transform(spss.begin(), spss.end(), std::back_inserter(sp),
                 [&](vector<SizeParity> sps) { return sps[p.dimension]; });
  BOOST_TEST(sp == sp1D);
};

BOOST_FIXTURE_TEST_CASE(test_sub_sizeparity_periodic, QF<Q1DPeriodic>) {
  test_sub_sizeparity_info_list_helper(partitioning, partitioning1D);
}

BOOST_FIXTURE_TEST_CASE(test_sub_sizeparity_open, QF<Q1DOpen>) {
  test_sub_sizeparity_info_list_helper(partitioning, partitioning1D);
}

BOOST_FIXTURE_TEST_CASE(test_sub_sizeparity_hbb, HBBF) {
  test_sub_sizeparity_info_list_helper(partitioning, partitioning1D);
}

BOOST_FIXTURE_TEST_CASE(test_sub_sizeparity_plain, PlainF) {
  test_sub_sizeparity_info_list_helper(partitioning, partitioning1D);
}

/*********************
 * idx_to_child_kind *
 *********************/

auto test_idx_to_child_kind_helper = [](auto p, auto p1D, int idx) {
  int child_kind = p.idx_to_child_kind(idx);
  int child_kind1D = p1D.idx_to_child_kind(idx);
  BOOST_TEST(child_kind == child_kind1D);
};

BOOST_DATA_TEST_CASE_F(QF<Q1DPeriodic>,                 //
                       test_idx_to_child_kind_periodic, //
                       bdata::xrange(4), idx) {
  test_idx_to_child_kind_helper(partitioning, partitioning1D, idx);
}

BOOST_DATA_TEST_CASE_F(QF<Q1DOpen>,                 //
                       test_idx_to_child_kind_open, //
                       bdata::xrange(4), idx) {
  test_idx_to_child_kind_helper(partitioning, partitioning1D, idx);
}

BOOST_DATA_TEST_CASE_F(HBBF,                       //
                       test_idx_to_child_kind_hbb, //
                       bdata::xrange(5), idx) {
  test_idx_to_child_kind_helper(partitioning, partitioning1D, idx);
}

BOOST_DATA_TEST_CASE_F(PlainF,                       //
                       test_idx_to_child_kind_plain, //
                       bdata::xrange(5), idx) {
  test_idx_to_child_kind_helper(partitioning, partitioning1D, idx);
}

/***********
 * max_idx *
 ***********/

BOOST_FIXTURE_TEST_CASE(test_max_idx_periodic, QF<Q1DPeriodic>) {
  BOOST_TEST(partitioning.max_idx_value() == partitioning1D.max_idx_value());
}

BOOST_FIXTURE_TEST_CASE(test_max_idx_open, QF<Q1DOpen>) {
  BOOST_TEST(partitioning.max_idx_value() == partitioning1D.max_idx_value());
}

BOOST_FIXTURE_TEST_CASE(test_max_idx_hbb, HBBF) {
  BOOST_TEST(partitioning.max_idx_value() == partitioning1D.max_idx_value());
}

BOOST_FIXTURE_TEST_CASE(test_max_idx_plain, PlainF) {
  BOOST_TEST(partitioning.max_idx_value() == partitioning1D.max_idx_value());
}

/*****************
 * coord_to_idxs *
 *****************/

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
  BOOST_TEST(r1D_ == r1D);
  // return std::make_tuple(r1D_, r1D);
};

BOOST_DATA_TEST_CASE_F(QF<Q1DPeriodic>,            //
                       test_coord_to_idx_periodic, //
                       bdata::xrange(-1, QSIZE + 1) *
                           bdata::xrange(-1, QSIZE + 1),
                       i, j) {

  test_coord_to_idx_helper(partitioning, partitioning1D, i, j);
}

BOOST_DATA_TEST_CASE_F(QF<Q1DOpen>,            //
                       test_coord_to_idx_open, //
                       bdata::xrange(-1, QSIZE + 1) *
                           bdata::xrange(-1, QSIZE + 1),
                       i, j) {
  test_coord_to_idx_helper(partitioning, partitioning1D, i, j);
}

BOOST_DATA_TEST_CASE_F(HBBF,                  //
                       test_coord_to_idx_hbb, //
                       bdata::xrange(-2, HSIZE + 2) *
                           bdata::xrange(-2, HSIZE + 2),
                       i, j) {
  test_coord_to_idx_helper(partitioning, partitioning1D, i, j);
}

BOOST_DATA_TEST_CASE_F(PlainF,                  //
                       test_coord_to_idx_plain, //
                       bdata::xrange(-2, HSIZE + 2) *
                           bdata::xrange(-2, HSIZE + 2),
                       i, j) {
  test_coord_to_idx_helper(partitioning, partitioning1D, i, j);
}

BOOST_AUTO_TEST_SUITE_END()
