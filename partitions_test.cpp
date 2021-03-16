#include "partitions.hpp"

int main() {
  {
    using hypercubes::slow::partitioning1D::p;
    {
      auto partition = p(382, p(100, p(9), p(1)), p(82, p(8), p(2)));
      std::cout << "Representing partition:" << *partition << std::endl;
      {
        auto indices351 = partition->indices(351);
        for (int i : indices351)
          std::cout << i << " ";
        std::cout << std::endl;
        assert((std::vector<int>{3, 6, 3} == indices351));
      }
      {
        auto indices238 = partition->indices(238);
        for (int i : indices238)
          std::cout << i << " ";
        std::cout << std::endl;
        assert((std::vector<int>{2, 4, 2} == indices238));
      }
      {
        auto indices381 = partition->indices(381);
        for (int i : indices381)
          std::cout << i << " ";
        std::cout << std::endl;
        assert((std::vector<int>{3, 10, 1} == indices381));
      }
    }
    {
      auto partition =
          p(382, p(100, p(10), p(0)), p(82, p(41, p(10), p(1)), p(0)));

      std::cout << "Representing partition:" << *partition << std::endl;

      {
        auto indices351 = partition->indices(351);
        for (int i : indices351)
          std::cout << i << " ";
        std::cout << std::endl;
        assert((std::vector<int>{3, 1, 1, 0} == indices351));
      }
      {
        auto indices238 = partition->indices(238);
        for (int i : indices238)
          std::cout << i << " ";
        std::cout << std::endl;
        assert((std::vector<int>{2, 3, 8} == indices238));
      }
      {
        auto indices381 = partition->indices(381);
        for (int i : indices381)
          std::cout << i << " ";
        std::cout << std::endl;
        assert((std::vector<int>{3, 1, 4, 0} == indices381));
      }
    }
  }
  return 0;
}
