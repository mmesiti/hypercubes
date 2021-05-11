#include "partition_tree.hpp"
#include "test_utils.hpp"
#include "tree.hpp"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

using namespace hypercubes::slow;
using namespace hypercubes::slow::partitioners;
namespace bdata = boost::unit_test::data;
/**
 * Simple 1D test.
 * */
BOOST_AUTO_TEST_SUITE(test_partition_tree)

BOOST_AUTO_TEST_CASE(test_1D_2lvl_simple) {

  using partitioners::QOpen;
  using partitioners::QPeriodic;

  PartList ps{
      QPeriodic("MPI", 0, 2), //
      QOpen("Vector", 0, 2)   //
  };

  SizeParityD sp{{8, Parity::EVEN}};
  PTBuilder treeBuilder;
  PartitionTree t = treeBuilder(sp, ps);

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

  PTBuilder treeBuilder;
  PartitionTree t = treeBuilder(sp, ps);

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
  PTBuilder treeBuilder;
  PartitionTree t = treeBuilder(sp, PartList{
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
  PTBuilder treeBuilder;
  PartitionTree t = treeBuilder(sp,                              //
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
  PTBuilder treeBuilder;
  PartitionTree t = treeBuilder(sp,                              //
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
  PTBuilder treeBuilder;
  PartitionTree t = treeBuilder(sp,                              //
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
  vector<std::pair<Indices, Sizes>> in_out{{{0}, {11}},      //
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
  PTBuilder treeBuilder;
  PartitionTree t = treeBuilder(sp,                              //
                                PartList{QPeriodic("MPI", 0, 4), //
                                         QOpen("Vector", 0, 2),  //
                                         HBB("Halo", 0, 1),      //
                                         Plain("Remainder", 0)});

  for (auto &idx_size : in_out) {
    Indices idx = idx_size.first;
    Sizes exp_size = idx_size.second;
    Sizes size = get_sizes_from_idx(t, idx);
    BOOST_TEST(size == exp_size);
  }
}

BOOST_AUTO_TEST_CASE(test_get_max_idx_tree) {
  SizeParityD sp{{42, Parity::EVEN}};
  PTBuilder treeBuilder;
  PartitionTree t = treeBuilder(sp,                              //
                                PartList{QPeriodic("MPI", 0, 4), //
                                         QOpen("Vector", 0, 2),  //
                                         HBB("Halo", 0, 1),      //
                                         Plain("Remainder", 0)});
  TreeP<int> expmaxtree = mt(4, {mt(2, {mt(5, {mt(1, {}),     //
                                               mt(1, {}),     //
                                               mt(4, {}),     //
                                               mt(1, {}),     //
                                               mt(1, {})}),   //
                                        mt(5, {mt(1, {}),     //
                                               mt(1, {}),     //
                                               mt(3, {}),     //
                                               mt(1, {}),     //
                                               mt(1, {})})}), //
                                 mt(2, {mt(5, {mt(1, {}),     //
                                               mt(1, {}),     //
                                               mt(4, {}),     //
                                               mt(1, {}),     //
                                               mt(1, {})}),   //
                                        mt(5, {mt(1, {}),     //
                                               mt(1, {}),     //
                                               mt(3, {}),     //
                                               mt(1, {}),     //
                                               mt(1, {})})}), //
                                 mt(2, {mt(5, {mt(1, {}),     //
                                               mt(1, {}),     //
                                               mt(4, {}),     //
                                               mt(1, {}),     //
                                               mt(1, {})}),   //
                                        mt(5, {mt(1, {}),     //
                                               mt(1, {}),     //
                                               mt(3, {}),     //
                                               mt(1, {}),     //
                                               mt(1, {})})}), //
                                 mt(2, {mt(5, {mt(1, {}),     //
                                               mt(1, {}),     //
                                               mt(3, {}),     //
                                               mt(1, {}),     //
                                               mt(1, {})}),   //
                                        mt(5, {mt(1, {}),     //
                                               mt(1, {}),     //
                                               mt(2, {}),     //
                                               mt(1, {}),     //
                                               mt(1, {})})})});

  TreeP<int> maxtree = get_max_idx_tree(t);

  BOOST_TEST(*expmaxtree == *maxtree);
}

BOOST_AUTO_TEST_CASE(test_get_max_idx_tree_weo) {
  SizeParityD sp{{11, Parity::EVEN}};
  PTBuilder treeBuilder;
  PartitionTree t = treeBuilder(sp,                                      //
                                PartList{QOpen("Vector", 0, 2),          //
                                         HBB("Halo", 0, 1),              //
                                         partitioners::EO("EO", {true}), //
                                         Plain("Remainder", 1),          //
                                         Plain("Remainder", 0)});

  TreeP<int> a;
  TreeP<int> expmaxtree = mt(2, {mt(5, {mt(2, {mt(0, {}),            //
                                               mt(1, {mt(1, {})})}), //
                                        mt(2, {mt(1, {mt(1, {})}),   //
                                               mt(0, {})}),          //
                                        mt(2, {mt(2, {mt(1, {}),     //
                                                      mt(1, {})}),   //
                                               mt(2, {mt(1, {}),     //
                                                      mt(1, {})})}), //
                                        mt(2, {mt(0, {}),            //
                                               mt(1, {mt(1, {})})}),
                                        mt(2, {mt(1, {mt(1, {})}), //
                                               mt(0, {})})}),
                                 mt(5, {mt(2, {mt(0, {}),            //
                                               mt(1, {mt(1, {})})}), //
                                        mt(2, {mt(1, {mt(1, {})}),   //
                                               mt(0, {})}),
                                        mt(2, {mt(1, {mt(1, {})}),   //
                                               mt(2, {mt(1, {}),     //
                                                      mt(1, {})})}), //
                                        mt(2, {mt(1, {mt(1, {})}),   //
                                               mt(0, {})}),          //
                                        mt(2, {mt(0, {}),            //
                                               mt(1, {mt(1, {})})})})});

  TreeP<int> maxtree = get_max_idx_tree(t);

  BOOST_TEST(*expmaxtree == *maxtree);
}

BOOST_DATA_TEST_CASE(test_validate_idx, //
                     bdata::make(std::vector<std::pair<Indices, bool>>{
                         {{0, 0, 0, 0}, {true}},  //
                         {{4}, {false}},          //
                         {{3, 2}, {false}},       //
                         {{3, 1}, {true}},        //
                         {{3, 1, 2, 1}, {true}},  //
                         {{3, 1, 2, 2}, {false}}, //
                         {{3, 0, 0, 0}, {true}},  //
                         {{3, 0, 0, 1}, {false}}, //
                         {{3, 0, 2, 2}, {true}},  //
                         {{3, 0, 2, 3}, {false}}, //
                         {{1, 0, 2, 3}, {true}},  //
                         {{1, 0, 2, 4}, {false}}  //
                     }),
                     idx_valid) {
  SizeParityD sp{{42, Parity::EVEN}};
  PTBuilder treeBuilder;
  PartitionTree t = treeBuilder(sp,                              //
                                PartList{QPeriodic("MPI", 0, 4), //
                                         QOpen("Vector", 0, 2),  //
                                         HBB("Halo", 0, 1),      //
                                         Plain("Remainder", 0)});

  Indices idx = idx_valid.first;
  bool expvalid = idx_valid.second;

  bool valid = validate_idx(t, idx);
  BOOST_TEST(valid == expvalid);
}

BOOST_AUTO_TEST_CASE(test_validate_idx_weo) {
  SizeParityD sp{{42, Parity::EVEN}};
  PTBuilder treeBuilder;
  PartitionTree t = treeBuilder(sp, //
                                PartList{
                                    QPeriodic("MPI", 0, 4),         //
                                    QOpen("Vector", 0, 2),          //
                                    HBB("Halo", 0, 1),              //
                                    partitioners::EO("EO", {true}), //
                                    Plain("Remainder-extra", 1),
                                    Plain("Remainder", 0),
                                });

  std::vector<std::pair<Indices, bool>> in_out{
      {{0, 0, 0, 0}, {true}},     //
      {{4}, {false}},             //
      {{3, 2}, {false}},          //
      {{3, 1}, {true}},           //
      {{3, 1, 2, 0}, {true}},     //
      {{3, 1, 2, 1}, {true}},     //
      {{3, 1, 2, 1, 0}, {true}},  //
      {{3, 1, 2, 1, 1}, {false}}, //
      {{3, 0, 2, 1, 0}, {true}},  //
      {{3, 0, 2, 1, 1}, {false}}, //
      {{3, 0, 1, 1, 0}, {true}},  //
      {{3, 0, 1, 0, 0}, {false}}  //
  };
  for (const auto &idx_valid : in_out) {
    Indices idx = idx_valid.first;
    bool expvalid = idx_valid.second;

    bool valid = validate_idx(t, idx);
    BOOST_TEST(valid == expvalid);
  }
}

BOOST_AUTO_TEST_CASE(test_partition_limits_1D) {
  vector<std::pair<Indices, std::pair<int, int>>> in_out{{{0}, {0, 16}},     //
                                                         {{1}, {16, 32}},    //
                                                         {{0, 0}, {0, 8}},   //
                                                         {{0, 1}, {8, 16}},  //
                                                         {{1, 0}, {16, 24}}, //
                                                         {{1, 1}, {24, 32}}};

  PartList partitioners{QPeriodic("MPI1", 0, 2), //
                        QOpen("VECTOR1", 0, 2),  //
                        Plain("Plain", 0)};

  SizeParityD sp{{32, Parity::EVEN}};

  PTBuilder tree_builder;
  PartitionTree t = tree_builder(sp, partitioners);

  for (auto idxs_limits : in_out) {
    Indices idxs = idxs_limits.first;
    auto explimits = idxs_limits.second;

    bool success = explimits == get_partition_limits(t, idxs)[0];
    BOOST_TEST(success);
  }
}

BOOST_AUTO_TEST_CASE(test_partition_limits_2D) {
  using PairVector = std::vector<std::pair<int, int>>;
  vector<std::pair<Indices, PairVector>> in_out{
      {{0}, {{0, 21}, {0, 42}}},               //
      {{0, 0}, {{0, 21}, {0, 21}}},            //
      {{0, 0, 0, 0}, {{0, 11}, {0, 11}}},      //
      {{1, 0, 1, 1}, {{32, 42}, {11, 21}}},    //
      {{1, 1, 0, 1}, {{21, 32}, {32, 42}}},    //
      {{1, 1, 0, 1, 0}, {{20, 21}, {32, 42}}}, //
      {{1, 1, 0, 1, 0, 2}, {{20, 21}, {33, 41}}}};

  enum { X, Y };
  PartList partitioners{QPeriodic("MPI X", X, 2), //
                        QPeriodic("MPI Y", Y, 2), //
                        QOpen("VECTOR X", X, 2),  //
                        QOpen("VECTOR Y", Y, 2),  //
                        HBB("HBB X", X, 1),       //
                        HBB("HBB Y", Y, 1),       //
                        Plain("Leaf X", X),       //
                        Plain("Leaf Y", Y)};
  SizeParityD sp{{42, Parity::EVEN}, {42, Parity::EVEN}};

  PTBuilder tree_builder;
  PartitionTree t = tree_builder(sp, partitioners);

  for (auto idxs_limits : in_out) {
    Indices idxs = idxs_limits.first;
    auto explimits = idxs_limits.second;

    bool success = explimits == get_partition_limits(t, idxs);
    BOOST_TEST(success);
  }
}
BOOST_AUTO_TEST_SUITE_END()
