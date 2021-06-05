#include "fixtures.hpp"
#include <cassert>

using namespace hypercubes::slow::partitioners;
namespace hypercubes {
namespace slow {
namespace internals {
using namespace partitioner_makers;

std::vector<int> _assemble(const std::string &tens, const std::string &units) {

  auto ichartoi = [](char c) { return int(c - '0'); };
  assert(tens.size() == units.size());
  std::vector<int> res;
  for (int i = 0; i < tens.size(); ++i)
    res.push_back(ichartoi(tens[i]) * 10 + //
                  ichartoi(units[i]));
  return res;
}
std::vector<int> get_bulk_sites42() {
  std::string tens = "00000001111112222223333334";
  std::string units = "12347892345890345690145690";
  return _assemble(tens, units);
}
std::vector<int> get_border_sites42() {
  std::string tens = "0001111222233334";
  std::string units = "0560167127823781";
  return _assemble(tens, units);
}

Part4DF::Part4DF()
    : sp{{42, Parity::EVEN}, //
         {42, Parity::EVEN}, //
         {42, Parity::EVEN}, //
         {42, Parity::EVEN}},
      partitioners{QPeriodic("MPI X", X, 4),           //
                   QPeriodic("MPI Y", Y, 4),           //
                   QPeriodic("MPI Z", Z, 4),           //
                   QPeriodic("MPI T", T, 4),           //
                   QOpen("Vector X", X, 2),            //
                   QOpen("Vector Y", Y, 2),            //
                   QOpen("Vector Z", Z, 2),            //
                   QOpen("Vector T", T, 2),            //
                   HBB("Halo X", X, 1),                //
                   HBB("Halo Y", Y, 1),                //
                   HBB("Halo Z", Z, 1),                //
                   HBB("Halo T", T, 1),                //
                   EO("EO", {true, true, true, true}), //
                   Plain("Remainder", EXTRA),          //
                   Site()},                            //
      t(get_partition_treeM(sp, partitioners)),        //
      bulk_sites(get_bulk_sites42()),                  //
      border_sites(get_border_sites42()) {}

Part4DFWLocal::Part4DFWLocal()
    : sp{{42, Parity::EVEN}, //
         {42, Parity::EVEN}, //
         {42, Parity::EVEN}, //
         {42, Parity::EVEN}, //
         {9, Parity::NONE},  //
    },
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
                   EO("EO", {true, true, true, true, false}), //
                   Plain("Local",     LOCAL),                        //
                   Plain("Remainder", EXTRA),                        //
                   Site()},                            //
      t(get_partition_treeM(sp, partitioners)),                              //
      bulk_sites(get_bulk_sites42()),                                //
      border_sites(get_border_sites42()) {}

Part1D42::Part1D42()
    : sp{{42, Parity::EVEN}},
      partitioners{
          QPeriodic("MPI X", X, 4),            //
          QOpen("Vector X", X, 2),             //
          HBB("Halo X", X, 1),                 //
          EO("EO", {true}),                    //
          Plain("Remainder", EXTRA),           //
          Site(),                              //
      },                                       //
      t(get_partition_tree(sp, partitioners)), //
      bulk_sites(get_bulk_sites42()),          //
      border_sites(get_border_sites42()),      //
      haloonly1D(get_haloonly1D()) {}

vector<std::pair<int, Indices>> Part1D42::get_haloonly1D() {
  vector<std::pair<int, Indices>> res;

  auto reorder_sites = [](vector<int> _border_sites) {
    int i = 1, step = 0;
    vector<int> out;
    while (i < _border_sites.size()) {
      out.push_back(_border_sites[i]);
      i += (step % 2 == 0) ? 3 : -1;
      ++step;
    }
    return out;
  };

  auto get_halo_idx = [&](int _bs) {
    auto itwg = get_indices_tree_with_ghosts(t, Coordinates{_bs});
    return get_relevant_indices_flat(itwg);
  };

  for (auto bs : reorder_sites(border_sites)) {
    for (auto halodim_idx : get_halo_idx(bs)) {
      int halodim;
      Indices idx;
      std::tie(halodim, idx) = halodim_idx;
      if (halodim)
        res.push_back(std::make_pair(bs, idx));
    }
  }
  return res;
}

} // namespace internals
} // namespace slow
} // namespace hypercubes
