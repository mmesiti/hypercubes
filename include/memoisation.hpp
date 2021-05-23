#ifndef MEMOISATION_H_
#define MEMOISATION_H_
#include <functional>
#include <map>
#include <tuple>

namespace hypercubes {
namespace slow {

template <class Out, class... Args> class Memoiser {
private:
  std::map<std::tuple<Args...>, Out> cache;
  std::function<Out(std::function<Out(Args...)>, Args...)> f;

public:
  template <class FF> Memoiser(FF _f) : f(_f){};
  Out operator()(Args... input) {
    auto input_key = std::make_tuple(input...);
    if (cache.find(input_key) == cache.end()) {
      auto out = f([this](Args... args) { return (*this)(args...); }, input...);
      cache[input_key] = out;
    }
    return cache[input_key];
  }
};

} // namespace slow

} // namespace hypercubes

#endif // MEMOISATION_H_
