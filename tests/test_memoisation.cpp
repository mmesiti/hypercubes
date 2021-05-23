#include "memoisation.hpp"
#include <functional>
#include <iostream>

int fib(std::function<int(int)> f, int i) {
  if (i == 0)
    return 0;
  if (i == 1)
    return 1;
  return f(i - 1) + f(i - 2);
}
using namespace hypercubes::slow;

int main() {

  // auto fib = [](auto &&f, int i) -> double {
  //  if (i == 0)
  //    return 0;
  //  if (i == 1)
  //    return 1;
  //  return f(i - 1) + f(i - 2);
  //};

  Memoiser<int, int> R(fib);

  for (int i = 0; i < 10; ++i)
    std::cout << R(i) << std::endl;
}
