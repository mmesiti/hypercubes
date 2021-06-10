#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <iostream>

#include "api/memory_layout.hpp"
#include "geometry/geometry.hpp"
#include "partitioners/partitioners.hpp"
#include "selectors/partition_predicates.hpp"
#include "trees/partition_tree.hpp"
#include "trees/tree.hpp"

using namespace hypercubes::slow;
namespace pm = hypercubes::slow::partitioner_makers;

struct GridLikeBase {

  enum { X, Y, Z, T, MATROW, MATCOL, EXTRA };
  vector<int> nonspatial_dimensions{MATROW, MATCOL};
  Sizes sizes;
  PartList partitioners;
  PartitionTree partition_tree;

  GridLikeBase()
      : sizes({48, 48, 42, 42, 3, 3}),
        partitioners({
            pm::QPeriodic("MPI X", X, 4),                         //  0
            pm::QPeriodic("MPI Y", Y, 4),                         //  1
            pm::QPeriodic("MPI Z", Z, 4),                         //  2
            pm::QPeriodic("MPI T", T, 4),                         //  3
            pm::QOpen("Vector X", X, 2),                          //  4
            pm::QOpen("Vector Y", Y, 2),                          //  5
            pm::HBB("Halo X", X, 1),                              //  6
            pm::HBB("Halo Y", Y, 1),                              //  7
            pm::HBB("Halo Z", Z, 1),                              //  8
            pm::HBB("Halo T", T, 1),                              //  9
            pm::EO("EO", {true, true, true, true, false, false}), // 10
            pm::Plain("Local-matrow", MATROW),                    // 11
            pm::Plain("Local-matcol", MATCOL),                    // 12
            pm::Plain("Extra", EXTRA),                            // 13
            pm::Site()                                            // 14
        }),
        partition_tree(sizes, partitioners, nonspatial_dimensions) {
    std::cout << "Max and min depths: ";
    std::cout << internals::get_max_depth(partition_tree.get_internal());
    std::cout << " ";
    std::cout << internals::get_min_depth(partition_tree.get_internal());
    std::cout << "\n";
  }
};
struct GridLikeNChildren : public GridLikeBase {
  PartitionPredicate predicate;
  NChildrenTree nchildren_tree;
  GridLikeNChildren()
      : GridLikeBase(),                                //
        predicate(get_mpi_rank_predicate(partitioners, //
                                         {2, 3, 1, 1})),
        nchildren_tree(partition_tree, //
                       predicate){};
};
struct GridLikeSize : public GridLikeNChildren {
  SizeTree size_tree;
  GridLikeSize() : GridLikeNChildren(), size_tree(nchildren_tree){};
};
struct GridLikeOffset : public GridLikeSize {
  OffsetTree offset_tree;
  GridLikeOffset() : GridLikeSize(), offset_tree(size_tree){};
};

struct GridLikeBase2D {

  enum { X, Y, MATROW, EXTRA };
  vector<int> nonspatial_dimensions{MATROW};
  Sizes sizes;
  PartList partitioners;
  PartitionTree partition_tree;

  GridLikeBase2D()
      : sizes({48, 48, 3}), partitioners({
                                pm::QPeriodic("MPI X", X, 4),      // 0
                                pm::QPeriodic("MPI Y", Y, 4),      // 1
                                pm::QOpen("Vector X", X, 2),       // 2
                                pm::QOpen("Vector Y", Y, 2),       // 3
                                pm::HBB("Halo X", X, 1),           // 4
                                pm::HBB("Halo Y", Y, 1),           // 5
                                pm::EO("EO", {true, true, false}), // 6
                                pm::Plain("Local-matrow", MATROW), // 7
                                pm::Plain("Extra", EXTRA),         // 8
                                pm::Site()                         // 9
                            }),
        partition_tree(sizes, partitioners, nonspatial_dimensions) {}
};
struct GridLike2DNChildren : public GridLikeBase2D {
  PartitionPredicate predicate;
  NChildrenTree nchildren_tree;
  GridLike2DNChildren()
      : GridLikeBase2D(), //
        predicate(get_mpi_rank_predicate(partitioners, {2, 1})),
        nchildren_tree(partition_tree, //
                       predicate){};
};
struct GridLike2DSize : public GridLike2DNChildren {
  SizeTree size_tree;
  GridLike2DSize() : GridLike2DNChildren(), size_tree(nchildren_tree){};
};

struct GridLikeBase1D {

  enum { X, MATROW, EXTRA };
  vector<int> nonspatial_dimensions{MATROW};
  Sizes sizes;
  PartList partitioners;
  PartitionTree partition_tree;

  GridLikeBase1D()
      : sizes({48, 3}), partitioners({
                            pm::QPeriodic("MPI X", X, 4),      // 0
                            pm::QOpen("Vector X", X, 2),       // 1
                            pm::HBB("Halo X", X, 1),           // 2
                            pm::EO("EO", {true, false}),       // 3
                            pm::Plain("Local-matrow", MATROW), // 4
                            pm::Plain("Extra", EXTRA),         // 5
                            pm::Site()                         // 6
                        }),
        partition_tree(sizes, partitioners, nonspatial_dimensions) {}
};
struct GridLike1DNChildren : public GridLikeBase1D {
  PartitionPredicate predicate;
  NChildrenTree nchildren_tree;
  GridLike1DNChildren()
      : GridLikeBase1D(), //
        predicate(get_mpi_rank_predicate(partitioners, {2})),
        nchildren_tree(partition_tree, //
                       predicate){};
};
struct GridLike1DSize : public GridLike1DNChildren {
  SizeTree size_tree;
  GridLike1DSize() : GridLike1DNChildren(), size_tree(nchildren_tree){};
};
struct GridLike1DOffset : public GridLike1DSize {
  OffsetTree offset_tree;
  GridLike1DOffset() : GridLike1DSize(), offset_tree(size_tree){};
};

BOOST_AUTO_TEST_SUITE(test_grid_like)
// PartitionTree
BOOST_FIXTURE_TEST_CASE(test_grid_like_get_indices, GridLikeBase) {
  Sizes sizes{48, 48, 42, 42, 3, 3};
  SizeParityD spd = add_parity(sizes, {MATROW, MATCOL});
  internals::PartitionTree pt =
      internals::get_partition_treeM(spd, partitioners);
  Coordinates xs{3, 23, 39, 15, 1, 2};
  Indices idxs = internals::get_real_indices(pt, xs);
  BOOST_TEST(idxs == partition_tree.get_indices(xs));
}
BOOST_FIXTURE_TEST_CASE(test_grid_like_get_indices_wg, GridLikeBase) {
  Sizes sizes{48, 48, 42, 42, 3, 3};
  SizeParityD spd = add_parity(sizes, {MATROW, MATCOL});
  internals::PartitionTree pt =
      internals::get_partition_treeM(spd, partitioners);
  Coordinates xs{3, 23, 39, 15, 1, 2};
  auto tree_indices = internals::get_indices_tree_with_ghosts(pt, xs);
  auto idxs = internals::get_relevant_indices_flat(tree_indices);

  BOOST_TEST(idxs == partition_tree.get_indices_wg(xs));
}
BOOST_FIXTURE_TEST_CASE(test_grid_like_get_coordinates, GridLikeBase) {
  Coordinates xs{3, 23, 39, 15, 1, 2};
  Indices idxs = partition_tree.get_indices(xs);

  BOOST_TEST(xs == partition_tree.get_coordinates(idxs));
}
// NChildrenTree
BOOST_FIXTURE_TEST_CASE(test_nchildren_tree_get_subtree, GridLikeNChildren) {

  NChildrenTree subtree_eo = nchildren_tree.get_subtree(Indices{
      2, 3, 1, 1, // MPI
                  // coordinates: 24,36,11,11
      0, 1,       // Vector
                  // coordinates: 24,42,11,11
      1, 1, 1, 1, // HBB
                  // coordinates: 24,42,11,11
  });
  // site is EVEN
  NChildrenTree subtree_e = subtree_eo.get_subtree(Indices{0});
  NChildrenTree subtree_o = subtree_eo.get_subtree(Indices{1});
  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };
  using internals::mt;
  auto exp_e = mt(mp(0, 3), // site is even
                  {mt(mp(0, 3), {mt(mp(0, 1), {mt(mp(0, 1), {})}),   //
                                 mt(mp(1, 1), {mt(mp(0, 1), {})}),   //
                                 mt(mp(2, 1), {mt(mp(0, 1), {})})}), //
                   mt(mp(1, 3), {mt(mp(0, 1), {mt(mp(0, 1), {})}),   //
                                 mt(mp(1, 1), {mt(mp(0, 1), {})}),   //
                                 mt(mp(2, 1), {mt(mp(0, 1), {})})}), //
                   mt(mp(2, 3), {mt(mp(0, 1), {mt(mp(0, 1), {})}),   //
                                 mt(mp(1, 1), {mt(mp(0, 1), {})}),   //
                                 mt(mp(2, 1), {mt(mp(0, 1), {})})})});

  // there are no sites
  auto exp_o = mt(mp(1, 3),                          // no odd sites
                  {mt(mp(0, 3), {mt(mp(0, 0), {}),   //
                                 mt(mp(1, 0), {}),   //
                                 mt(mp(2, 0), {})}), //
                   mt(mp(1, 3), {mt(mp(0, 0), {}),   //
                                 mt(mp(1, 0), {}),   //
                                 mt(mp(2, 0), {})}), //
                   mt(mp(2, 3), {mt(mp(0, 0), {}),   //
                                 mt(mp(1, 0), {}),   //
                                 mt(mp(2, 0), {})})});

  BOOST_TEST(*(subtree_e.get_internal()) == *exp_e);
  BOOST_TEST(*(subtree_o.get_internal()) == *exp_o);
}
BOOST_FIXTURE_TEST_CASE(test_nchildren_dot, GridLike1DNChildren) {

  auto nchildren_tree_2 = partition_tree.nchildren_tree(predicate);
  BOOST_TEST(*nchildren_tree_2.get_internal() ==
             *nchildren_tree.get_internal());
}
BOOST_FIXTURE_TEST_CASE(test_get_nchildren, GridLike1DNChildren) {
  BOOST_TEST(nchildren_tree.get_nchildren(Indices{2, 0}) == 5);
}
BOOST_FIXTURE_TEST_CASE(test_nchildren_permute2D, GridLike2DNChildren) {
  vector<std::string> permuted_level_names{
      "MPI X",        // 0
      "MPI Y",        // 1
      "EO",           // 2
      "Local-matrow", // 3
      "Halo X",       // 4
      "Halo Y",       // 5
      "Extra",        // 6
      "Vector X",     // 7
      "Vector Y",     // 8
      "Site",         // 9
  };
  NChildrenTree permuted_tree = nchildren_tree.permute(permuted_level_names);
  // To level 4 - Halo X
  BOOST_TEST(permuted_tree.get_nchildren(Indices{2, 1, 0, 0}) == 5);
  // To level 7 - Vector X
  BOOST_TEST(permuted_tree.get_nchildren(Indices{2, 1, 0, 0, 2, 2, 0}) == 2);
  // To level 8 - Vector Y
  BOOST_TEST(permuted_tree.get_nchildren(Indices{2, 1, 0, 0, 2, 2, 0, 0}) == 2);
  // To level 9 - Site
  BOOST_TEST(permuted_tree.get_nchildren(Indices{2, 1, 0, 0, 2, 2, 0, 0, 1}) ==
             1);
  // Invalid - out of pruned
  BOOST_CHECK_THROW(permuted_tree.get_nchildren(Indices{2, 2, 0, 0}),
                    std::invalid_argument);
}
BOOST_FIXTURE_TEST_CASE(test_nchildren_permute, GridLikeNChildren) {
  vector<std::string> permuted_level_names{
      "MPI X",        //  0
      "MPI Y",        //  1
      "MPI Z",        //  3
      "MPI T",        //  4
      "EO",           //  5
      "Local-matrow", //  6
      "Local-matcol", //  7
      "Halo X",       //  8
      "Halo Y",       //  9
      "Halo Z",       // 10
      "Halo T",       // 11
      "Extra",        // 12
      "Vector X",     // 13
      "Vector Y",     // 14
      "Site",         // 15
  };
  NChildrenTree permuted_tree = nchildren_tree.permute(permuted_level_names);
  // 4 - EO
  BOOST_TEST(permuted_tree.get_nchildren(Indices{2, 3, 1, 1}) == 2);
  // 5 - Local-matrow
  BOOST_TEST(permuted_tree.get_nchildren(Indices{2, 3, 1, 1, 0}) == 3);
  // 6 - Local-matcol
  BOOST_TEST(permuted_tree.get_nchildren(Indices{2, 3, 1, 1, 1, 2}) == 3);
  // 7 - HBB X
  BOOST_TEST(permuted_tree.get_nchildren(Indices{2, 3, 1, 1, 1, 2, 2}) == 5);
  // 12 - extra: sites are all even:
  // (24+5,36+5,11+10,11+10) = (29,41,21,21)
  // ... (3 other sites of the same parity)
  BOOST_TEST(permuted_tree.get_nchildren(Indices{2, 3, 1, 1, 1, 2, 2, //
                                                 3, 3, 3, 3}) == 0);
  // 12 - extra: sites are all odd:
  // (24,36+5,11,11) = (24,41,21,21)
  // ... (3 other sites of the same parity)
  BOOST_TEST(permuted_tree.get_nchildren(Indices{2, 3, 1, 1, 1, 2, 2, //
                                                 1, 3, 1, 1}) == 1);
  // 12 - extra: sites are half even and half odd:
  // 4*4*9*9 total sites / 2
  BOOST_TEST(permuted_tree.get_nchildren(Indices{2, 3, 1, 1, 1, 2, 2, //
                                                 2, 2, 2, 2}) ==
             4 * 4 * 9 * 9 / 2);
  // 12 - extra:
  // number of sites is odd
  // majority is even because the parity of the center is even.
  // Center:
  // (24,36,11+5,11+5) = (24,41,16,16)
  // 9*9 total sites / 2
  // ODD:
  BOOST_TEST(permuted_tree.get_nchildren(Indices{2, 3, 1, 1, 1, 2, 2, //
                                                 1, 1, 2, 2}) == 9 * 9 / 2);
  // EVEN:
  BOOST_TEST(permuted_tree.get_nchildren(Indices{2, 3, 1, 1, 0, 2, 2, //
                                                 1, 1, 2, 2}) == 9 * 9 / 2 + 1);
}
// Size
BOOST_FIXTURE_TEST_CASE(test_size_dot, GridLike1DSize) {
  auto size_tree_2 = nchildren_tree.size_tree();
  BOOST_TEST(*size_tree_2.get_internal() == *size_tree.get_internal());
}
BOOST_FIXTURE_TEST_CASE(test_size_tree_get_subtree, GridLikeSize) {

  SizeTree subtree_eo = size_tree.get_subtree(Indices{
      2, 3, 1, 1, // MPI
                  // coordinates: 24,36,11,11
      0, 1,       // Vector
                  // coordinates: 24,42,11,11
      1, 1, 1, 1, // HBB
                  // coordinates: 24,42,11,11
  });
  // site is EVEN
  SizeTree subtree_e = subtree_eo.get_subtree(Indices{0});
  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };
  using internals::mt;
  auto exp_e = mt(mp(0, 9), // site is even
                  {mt(mp(0, 3), {mt(mp(0, 1), {mt(mp(0, 1), {})}),   //
                                 mt(mp(1, 1), {mt(mp(0, 1), {})}),   //
                                 mt(mp(2, 1), {mt(mp(0, 1), {})})}), //
                   mt(mp(1, 3), {mt(mp(0, 1), {mt(mp(0, 1), {})}),   //
                                 mt(mp(1, 1), {mt(mp(0, 1), {})}),   //
                                 mt(mp(2, 1), {mt(mp(0, 1), {})})}), //
                   mt(mp(2, 3), {mt(mp(0, 1), {mt(mp(0, 1), {})}),   //
                                 mt(mp(1, 1), {mt(mp(0, 1), {})}),   //
                                 mt(mp(2, 1), {mt(mp(0, 1), {})})})});

  BOOST_TEST(*(subtree_e.get_internal()) == *exp_e);
  // there are no odd sites
  BOOST_CHECK_THROW(subtree_eo.get_subtree(Indices{1}), std::invalid_argument);
}
BOOST_FIXTURE_TEST_CASE(test_get_size, GridLike2DSize) {
  BOOST_TEST(size_tree.get_size(Indices{2, 1}) //
             == 2 *                            // vec X
                    2 *                        // vec Y
                    8 *                        // 1+6+1 , hbb X
                    8 *                        // 1+6+1 , hbb 2
                    3);                        // local dof
}
// Offset
BOOST_FIXTURE_TEST_CASE(test_offset_dot, GridLike1DOffset) {
  auto offset_tree_2 = size_tree.offset_tree();
  BOOST_TEST(*offset_tree.get_internal() == *offset_tree_2.get_internal());
}
BOOST_FIXTURE_TEST_CASE(test_offset_tree_get_subtree, GridLikeOffset) {

  OffsetTree subtree_eo = offset_tree.get_subtree(Indices{
      2, 3, 1, 1, // MPI
                  // coordinates: 24,36,11,11
      0, 1,       // Vector
                  // coordinates: 24,42,11,11
      1, 1, 1, 1, // HBB
                  // coordinates: 24,42,11,11
  });
  // site is EVEN
  OffsetTree subtree_e = subtree_eo.get_subtree(Indices{0});
  int main_offset = subtree_e.get_offset({});
  int mo = main_offset;
  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };
  using internals::mt;
  auto exp_e =
      mt(mp(0, mo),                                          // site is even
         {mt(mp(0, mo), {mt(mp(0, mo), {mt(mp(0, mo), {})}), //
                         mt(mp(1, mo + 1), {mt(mp(0, mo + 1), {})}),       //
                         mt(mp(2, mo + 2), {mt(mp(0, mo + 2), {})})}),     //
          mt(mp(1, mo + 3), {mt(mp(0, mo + 3), {mt(mp(0, mo + 3), {})}),   //
                             mt(mp(1, mo + 4), {mt(mp(0, mo + 4), {})}),   //
                             mt(mp(2, mo + 5), {mt(mp(0, mo + 5), {})})}), //
          mt(mp(2, mo + 6), {mt(mp(0, mo + 6), {mt(mp(0, mo + 6), {})}),   //
                             mt(mp(1, mo + 7), {mt(mp(0, mo + 7), {})}),   //
                             mt(mp(2, mo + 8), {mt(mp(0, mo + 8), {})})})});

  BOOST_TEST(*(subtree_e.get_internal()) == *exp_e);
  // there are no odd sites
  BOOST_CHECK_THROW(subtree_eo.get_subtree(Indices{1}), std::invalid_argument);
}
BOOST_FIXTURE_TEST_CASE(test_offset_shift_subtree, GridLikeOffset) {

  OffsetTree subtree_eo = offset_tree.get_subtree(Indices{
      2, 3, 1, 1, // MPI
                  // coordinates: 24,36,11,11
      0, 1,       // Vector
                  // coordinates: 24,42,11,11
      1, 1, 1, 1, // HBB
                  // coordinates: 24,42,11,11
  });
  // site is EVEN
  OffsetTree subtree_e = subtree_eo.get_subtree(Indices{0});
  int main_offset = subtree_e.get_offset({});
  auto mp = [](auto a, auto b) { return std::make_pair(a, b); };
  using internals::mt;
  auto exp_e = mt(mp(0, 0), // site is even
                  {mt(mp(0, 0), {mt(mp(0, 0), {mt(mp(0, 0), {})}),   //
                                 mt(mp(1, 1), {mt(mp(0, 1), {})}),   //
                                 mt(mp(2, 2), {mt(mp(0, 2), {})})}), //
                   mt(mp(1, 3), {mt(mp(0, 3), {mt(mp(0, 3), {})}),   //
                                 mt(mp(1, 4), {mt(mp(0, 4), {})}),   //
                                 mt(mp(2, 5), {mt(mp(0, 5), {})})}), //
                   mt(mp(2, 6), {mt(mp(0, 6), {mt(mp(0, 6), {})}),   //
                                 mt(mp(1, 7), {mt(mp(0, 7), {})}),   //
                                 mt(mp(2, 8), {mt(mp(0, 8), {})})})});

  auto subtree_shifted = subtree_e.shift(-main_offset);
  BOOST_TEST(*(subtree_shifted.get_internal()) == *exp_e);
}

BOOST_AUTO_TEST_SUITE_END()
