#include "partition_class_tree.hpp"
#include "tree.hpp"
#include "utils.hpp"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

using namespace hypercubes::slow;
using namespace hypercubes::slow::partitioners;
namespace bdata = boost::unit_test::data;
/**
 * Simple 1D test.
 * */
BOOST_AUTO_TEST_SUITE(test_partition_class_tree)

BOOST_AUTO_TEST_CASE(test_1D_2lvl_simple) {

  using partitioners::QOpen;
  using partitioners::QPeriodic;

  PartList ps{
      QPeriodic("MPI", 0, 2), //
      QOpen("Vector", 0, 2)   //
  };

  SizeParityD sp{{8, Parity::EVEN}};
  PCTBuilder treeBuilder;
  PartitionClassTree t = treeBuilder(sp, ps);

  std::string comments0, comments1;
  {
    comments0 =
        Q1DPeriodic(SizeParity{8, Parity::EVEN}, 0, "MPI", 2).comments();
    comments1 = Q1DOpen(SizeParity{4, Parity::EVEN}, 0, "Vector", 2).comments();
  }

  BOOST_TEST(t->n->comments() == comments0);
  BOOST_TEST(t->children[0]->n->comments() == comments1);
}

BOOST_AUTO_TEST_CASE(test_1D_2lvl_repr) {

  using partitioners::QOpen;
  using partitioners::QPeriodic;

  PartList ps{
      QPeriodic("MPI", 0, 4), //
      QOpen("Vector", 0, 2)   //
  };

  SizeParityD sp{{42, Parity::EVEN}};

  PCTBuilder treeBuilder;
  PartitionClassTree t = treeBuilder(sp, ps);

  std::string repr = partition_class_tree_to_str(t, "", 5);

  std::string comments0;
  vector<std::string> comments1;

  {
    comments0 =
        Q1DPeriodic(SizeParity{42, Parity::EVEN}, 0, "MPI", 4).comments();
    comments1 = vector<std::string>{
        Q1DOpen(SizeParity{9, Parity::ODD}, 0, "Vector", 2).comments(),
        Q1DOpen(SizeParity{11, Parity::EVEN}, 0, "Vector", 2).comments(),
        Q1DOpen(SizeParity{11, Parity::ODD}, 0, "Vector", 2).comments()};
  }
  std::string prefix = "   ";
  std::string exprepr = std::string("+MPI") + "\n" +          //
                        " " + comments0 + "\n" +              //
                        prefix + "|+" + "Vector" + "\n" +     //
                        prefix + "| " + comments1[0] + "\n" + //
                        prefix + "|+" + "Vector" + "\n" +     //
                        prefix + "| " + comments1[1] + "\n" + //
                        prefix + " +" + "Vector" + "\n" +     //
                        prefix + "  " + comments1[2];

  BOOST_TEST(exprepr == repr);
}

BOOST_AUTO_TEST_CASE(test_get_indices_tree_simple) {
  SizeParityD sp{{42, Parity::EVEN},  //
                 {42, Parity::EVEN}}; //
  PCTBuilder treeBuilder;
  PartitionClassTree t = treeBuilder(sp, PartList{
                                             QPeriodic("X-MPI", 0, 4), //
                                             QPeriodic("Y-MPI", 1, 4), //
                                             QOpen("X-Vector", 0, 2),  //
                                             QOpen("Y-Vector", 1, 2),  //
                                             Plain("X-remainder", 0),  //
                                             Plain("Y-remainder", 1),  //
                                         });

  Coordinates xs{29, 20};

  TreeP<int> expected = mt(2, {                                  //
                               mt(1, {                           //
                                      mt(1, {                    //
                                             mt(1, {             //
                                                    mt(1, {      //
                                                           mt(3, //
                                                              {})})})})})});
  TreeP<int> idx = get_indices_tree(t, xs);

  BOOST_TEST(*expected == *idx);
}

BOOST_AUTO_TEST_CASE(test_get_indices_tree_wg_1D_nohalo) {

  SizeParityD sp{{42, Parity::EVEN}};
  PCTBuilder treeBuilder;
  PartitionClassTree t = treeBuilder(sp,                              //
                                     PartList{QPeriodic("MPI", 0, 4), //
                                              QOpen("Vector", 0, 2),  //
                                              Plain("Remainder", 0)});

  Coordinates xs{22};

  TreeP<GhostResult> idxt_gr = get_indices_tree_with_ghosts(t, xs);
  TreeP<int> idxt =
      nodemap<GhostResult, int>(idxt_gr, [](GhostResult gr) { return gr.idx; });
  vector<Indices> idxs = get_all_paths(idxt);
  decltype(idxs) relevant_idxs;
  std::copy_if(idxs.begin(), idxs.end(), std::back_inserter(relevant_idxs),
               [](auto v) { return v.size() == 4; });
  decltype(idxs) expected{
      {0, 2, 0, 0} // real
  };

  BOOST_TEST(expected == relevant_idxs);
}

BOOST_AUTO_TEST_CASE(test_get_indices_tree_wg_1D_hbb) {

  SizeParityD sp{{42, Parity::EVEN}};
  PCTBuilder treeBuilder;
  PartitionClassTree t = treeBuilder(sp,                              //
                                     PartList{QPeriodic("MPI", 0, 4), //
                                              QOpen("Vector", 0, 2),  //
                                              HBB("Halo", 0, 1),      //
                                              Plain("Remainder", 0)});

  Coordinates xs{22};

  TreeP<GhostResult> idxt_gr = get_indices_tree_with_ghosts(t, xs);
  TreeP<int> idxt = nodemap<GhostResult, int>(idxt_gr,             //
                                              [](GhostResult gr) { //
                                                return gr.idx;     //
                                              });
  vector<Indices> idxs = get_all_paths(idxt);
  decltype(idxs) relevant_idxs;
  std::copy_if(idxs.begin(), idxs.end(),          //
               std::back_inserter(relevant_idxs), //
               [](auto v) { return v.size() == 5; });
  decltype(idxs) expected{
      {0, 1, 1, 4, 0}, // ghost
      {0, 2, 0, 1, 0}  // real
  };

  BOOST_TEST(expected == relevant_idxs);
}

BOOST_DATA_TEST_CASE(test_get_coord_from_idx_roundtrip, //
                     bdata::xrange(0, 41), i) {
  SizeParityD sp{{42, Parity::EVEN}};
  PCTBuilder treeBuilder;
  PartitionClassTree t = treeBuilder(sp,                              //
                                     PartList{QPeriodic("MPI", 0, 4), //
                                              QOpen("Vector", 0, 2),  //
                                              HBB("Halo", 0, 1),      //
                                              Plain("Remainder", 0)});

  Coordinates xs{i};
  Indices idx = get_all_paths(get_indices_tree(t, xs))[0];
  Coordinates new_xs = get_coord_from_idx(t, idx);

  BOOST_TEST(new_xs == xs);
}

BOOST_AUTO_TEST_CASE(test_get_sizes_from_idx) {
  vector<std::tuple<Indices, Sizes>> in_out{{{0}, {11}},      //
                                            {{1}, {11}},      //
                                            {{2}, {11}},      //
                                            {{3}, {9}},       //
                                            {{0, 0}, {6}},    //
                                            {{0, 1}, {5}},    //
                                            {{0, 0, 0}, {1}}, //
                                            {{0, 0, 1}, {1}}, //
                                            {{0, 0, 2}, {4}}, //
                                            {{0, 0, 3}, {1}}, //
                                            {{0, 0, 4}, {1}}, //
                                            {{3, 0, 2}, {3}}, //
                                            {{3, 1, 2}, {2}}};
  SizeParityD sp{{42, Parity::EVEN}};
  PCTBuilder treeBuilder;
  PartitionClassTree t = treeBuilder(sp,                              //
                                     PartList{QPeriodic("MPI", 0, 4), //
                                              QOpen("Vector", 0, 2),  //
                                              HBB("Halo", 0, 1),      //
                                              Plain("Remainder", 0)});

  for (auto &idx_size : in_out) {
    Indices idx = std::get<0>(idx_size);
    Sizes exp_size = std::get<1>(idx_size);
    Sizes size = get_sizes_from_idx(t, idx);
    BOOST_TEST(size == exp_size);
  }
}

BOOST_AUTO_TEST_CASE(test_get_max_idx_tree) {
  SizeParityD sp{{42, Parity::EVEN}};
  PCTBuilder treeBuilder;
  PartitionClassTree t = treeBuilder(sp,                              //
                                     PartList{QPeriodic("MPI", 0, 4), //
                                              QOpen("Vector", 0, 2),  //
                                              HBB("Halo", 0, 1),      //
                                              Plain("Remainder", 0)});
  TreeP<int> expmaxtree = mt(4, {mt(2, {mt(5, {mt(1, {}),     //
                                               mt(1, {}),     //
                                               mt(2, {})}),   //
                                        mt(5, {mt(1, {}),     //
                                               mt(1, {}),     //
                                               mt(3, {})})}), //
                                 mt(2, {mt(5, {mt(1, {}),     //
                                               mt(1, {}),     //
                                               mt(3, {})}),   //
                                        mt(5, {mt(1, {}),     //
                                               mt(1, {}),     //
                                               mt(4, {})})}), //
                                 mt(2, {mt(5, {mt(1, {}),     //
                                               mt(1, {}),     //
                                               mt(3, {})}),   //
                                        mt(5, {mt(1, {}),     //
                                               mt(1, {}),     //
                                               mt(4, {})})})});

  TreeP<int> maxtree = get_max_idx_tree(t);

  BOOST_TEST(*expmaxtree == *maxtree);
}

BOOST_AUTO_TEST_SUITE_END()
