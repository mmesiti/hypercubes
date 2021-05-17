#include "fixtures.hpp"
#include <cassert>

using namespace hypercubes::slow::partitioners;
namespace hypercubes {
namespace slow {

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
                   Plain("Remainder", EXTRA),                        //
                   partitioners::Site()},                            //
      treeBuilder(),                                                 //
      t(treeBuilder(sp, partitioners)),                              //
      bulk_sites(get_bulk_sites42()),                                //
      border_sites(get_border_sites42()) {}

Part1D42::Part1D42()
    : sp{{42, Parity::EVEN}},
      partitioners{
          QPeriodic("MPI X", X, 4),       //
          QOpen("Vector X", X, 2),        //
          HBB("Halo X", X, 1),            //
          partitioners::EO("EO", {true}), //
          Plain("Remainder", EXTRA),      //
          partitioners::Site(),           //
      },                                  //
      treeBuilder(),                      //
      t(treeBuilder(sp, partitioners)),   //
      bulk_sites(get_bulk_sites42()),     //
      border_sites(get_border_sites42()) {}
} // namespace slow
} // namespace hypercubes
