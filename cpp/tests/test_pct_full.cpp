#include "partition_class_tree.hpp"
#include "tree.hpp"
#include "utils.hpp"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <cmath>

using namespace hypercubes::slow;
using namespace hypercubes::slow::partitioners;
using namespace data;
namespace bdata = boost::unit_test::data;

// Partitioning expected, for each dimension, until EO level:
// Site index:
// tens:    |0 0000 0 0 000 1 1 1111 1 1 112 2 2 2222 2 2 233 3 3 333 3 3 34 4|
// units:   |0 1234 5 6 789 0 1 2345 6 7 890 1 2 3456 7 8 901 2 3 456 7 8 90 1|
// Sizes,l0: 11               11               11               9
//          |0 0000 0 0 000 0|1 1111 1 1 111 1|2 2222 2 2 222 2|3 333 3 3 33 3|
// Sizes,l1: 6        5       6        5       6        5       5       4
//          |0 0000 0|1 111 1|0 0000 0|1 111 1|0 0000 0|1 111 1|0 000 0|1 11 1|
// brdr-blk:|1|2222|3|1|222|3|1|2222|3|1|222|3|1|2222|3|1|222|3|1|222|3|1|22|3|
// In-p idx:|0|0123|0|0|012|0|0|0123|0|0|012|0|0|0123|0|0|012|0|0|012|0|0|01|0|
// In-p sz:  1 4    1 1 3   1 1 4    1 1 3   1 1 4    1 1 3   1 1 3   1 1 2  1

struct Part4DF {

  enum { X, Y, Z, T, EXTRA };
  SizeParityD sp;
  PartList partitioners;
  PCTBuilder treeBuilder;
  PartitionClassTree t;
  std::vector<int> bulk_sites;
  std::vector<int> border_sites;

  std::vector<int> assemble(const std::string &tens, const std::string &units) {

    auto ichartoi = [](char c) { return int(c - '0'); };
    assert(tens.size() == units.size());
    std::vector<int> res;
    for (int i = 0; i < tens.size(); ++i)
      res.push_back(ichartoi(tens[i]) * 10 + //
                    ichartoi(units[i]));
    return res;
  }
  std::vector<int> get_bulk_sites() {
    std::string tens = "00000001111112222223333334";
    std::string units = "12347892345890345690145690";
    return assemble(tens, units);
  }
  std::vector<int> get_border_sites() {
    std::string tens = "0001111222233334";
    std::string units = "0560167127823781";
    return assemble(tens, units);
  }

  Part4DF()
      : sp{{42, Parity::EVEN}, //
           {42, Parity::EVEN}, //
           {42, Parity::EVEN}, //
           {42, Parity::EVEN}},
        partitioners{QPeriodic("MPI X", X, 4),                         //
                     QPeriodic("MPI Y", Y, 4),                         //
                     QPeriodic("MPI Z", Z, 4),                         //
                     QPeriodic("MPI T", T, 4),                         //
                     QOpen("Vector X", X, 2),                          //
                     QOpen("Vector Y", Y, 2),                          //
                     QOpen("Vector Z", Z, 2),                          //
                     QOpen("Vector T", T, 2),                          //
                     HBB("Halo X", X, 1),                              //
                     HBB("Halo Y", Y, 1),                              //
                     HBB("Halo Z", Z, 1),                              //
                     HBB("Halo T", T, 1),                              //
                     partitioners::EO("EO", {true, true, true, true}), //
                     Plain("Remainder", EXTRA)},                       //
        treeBuilder(),                                                 //
        t(treeBuilder(sp, partitioners)),                              //
        bulk_sites(get_bulk_sites()),                                  //
        border_sites(get_border_sites()) {}
};

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

  TreeP<int> idxt = get_indices_tree(t, xs);
  vector<int> idxs = get_all_paths(idxt)[0];

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
    ++xs;
  }
}

BOOST_AUTO_TEST_SUITE_END()
