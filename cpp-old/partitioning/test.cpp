#include "hbb.hpp"
#include "leaf.hpp"
#include "q.hpp"
#include "qr.hpp"

#include <initializer_list>

int main() {

  using hypercubes::slow::partitioning1D::hbb;
  using hypercubes::slow::partitioning1D::leaf;
  using hypercubes::slow::partitioning1D::q;
  using hypercubes::slow::partitioning1D::qr;

  auto check = [](auto partitioning, int idx,
                  std::initializer_list<int> expected_indices) {
    auto indices = partitioning->indices(idx);
    for (int i : indices)
      std::cout << i << " ";
    std::cout << std::endl;
    assert((std::vector<int>(expected_indices) == indices));
  };
  {

    auto partitioning = qr<0>("X/Node", 382,                //
                              qr<1>("socket-q", 100,        //
                                    leaf<2>("core-q", 9),   //
                                    leaf<2>("core-r", 1)),  //
                              qr<1>("socket-r", 82,         //
                                    leaf<2>("core-q", 8),   //
                                    leaf<2>("core-r", 2))); //
    std::cout << "Representing partition:" << *partitioning << std::endl;

    check(partitioning, 351, {3, 6, 3});
    check(partitioning, 238, {2, 4, 2});
    check(partitioning, 381, {3, 10, 1});
  }
  {
    auto partitioning = qr<0>("X/Node", 382,                      //
                              q<1>("Socket-q", 100,               //
                                   qr<2>("vector", 10,            //
                                         leaf<3>("core-q", 1),    //
                                         leaf<3>("core-r", 0))),  //
                              q<1>("Socket-r", 82,                //
                                   qr<2>("vector", 41,            //
                                         leaf<3>("core-q", 10),   //
                                         leaf<3>("core-r", 1)))); //

    std::cout << "Representing partition:\n" << *partitioning << std::endl;

    check(partitioning, 351, {3, 1, 1, 0});
    check(partitioning, 238, {2, 3, 8, 0});
    check(partitioning, 381, {3, 1, 4, 0});
  }

  {
    auto partitioning = qr<0>("X/Node", 140,                      //
                              q<1>("Socket-q", 32,                //
                                   hbb<2>("hbb", 16, 1,           //
                                          leaf<3>("bulk", 14),    //
                                          leaf<3>("border", 1))), //
                              q<1>("Socket-r", 12,                //
                                   hbb<2>("hbb", 12, 1,           //
                                          leaf<3>("bulk", 10),    //
                                          leaf<3>("border", 1))));

    std::cout << "Representing partition:\n" << *partitioning << std::endl;
    check(partitioning, 139, {4, 0, 3, 0});
    check(partitioning, 81, {2, 1, 2, 0});
  }
  return 0;
}
