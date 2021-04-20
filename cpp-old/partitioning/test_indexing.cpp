
#include "indexer.hpp"

#include <initializer_list>

int main() {
  using namespace hypercubes::slow::partitioning;

  {
    Indexer I;

    auto partitioning = I.qr<0>(140,                            //
                                I.q<1>(32,                      //
                                       I.hbb<2>(16, 1,          //
                                                I.leaf<3>(14),  //
                                                I.leaf<3>(1))), //
                                I.q<1>(12,                      //
                                       I.hbb<2>(12, 1,          //
                                                I.leaf<3>(10),  //
                                                I.leaf<3>(1))));

    std::cout << "Representing partition:" << *partitioning << std::endl;
  }
  return 0;
}
