#include "partitions.hpp"

int main() {
  {
    using hypercubes::slow::partitioning1D::p;
    {
      auto partitioning = p<0>(382,            //
                               p<1>(100,       //
                                    p<2>(9),   //
                                    p<2>(1)),  //
                               p<1>(82,        //
                                    p<2>(8),   //
                                    p<2>(2))); //
      std::cout << "Representing partition:" << *partitioning << std::endl;
      {
        auto indices351 = partitioning->indices(351);
        for (int i : indices351)
          std::cout << i << " ";
        std::cout << std::endl;
        assert((std::vector<int>{3, 6, 3} == indices351));
      }
      {
        auto indices238 = partitioning->indices(238);
        for (int i : indices238)
          std::cout << i << " ";
        std::cout << std::endl;
        assert((std::vector<int>{2, 4, 2} == indices238));
      }
      {
        auto indices381 = partitioning->indices(381);
        for (int i : indices381)
          std::cout << i << " ";
        std::cout << std::endl;
        assert((std::vector<int>{3, 10, 1} == indices381));
      }
    }
    {
      auto partitioning = p<0>(382,                  //
                               p<1>(100,             //
                                    p<2>(10,         //
                                         p<3>(1),    //
                                         p<3>(0)),   //
                                    p<2>(0,          //
                                         p<3>(1),    //
                                         p<3>(0))),  //
                               p<1>(82,              //
                                    p<2>(41,         //
                                         p<3>(10),   //
                                         p<3>(1)),   //
                                    p<2>(0,          //
                                         p<3>(1),    //
                                         p<3>(0)))); //

      std::cout << "Representing partition:" << *partitioning << std::endl;

      {
        auto indices351 = partitioning->indices(351);
        for (int i : indices351)
          std::cout << i << " ";
        std::cout << std::endl;
        assert((std::vector<int>{3, 1, 1, 0} == indices351));
      }
      {
        auto indices238 = partitioning->indices(238);
        for (int i : indices238)
          std::cout << i << " ";
        std::cout << std::endl;
        assert((std::vector<int>{2, 3, 8, 0} == indices238));
      }
      {
        auto indices381 = partitioning->indices(381);
        for (int i : indices381)
          std::cout << i << " ";
        std::cout << std::endl;
        assert((std::vector<int>{3, 1, 4, 0} == indices381));
      }
    }
  }
  return 0;
}
