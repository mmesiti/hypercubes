#ifndef UTILS_H_
#define UTILS_H_
#include <algorithm>
#include <vector>

namespace hypercubes {
namespace slow {
template <class Container> Container tail(const Container &idxs) {
  Container new_idxs;
  std::copy(idxs.begin() + 1, idxs.end(), std::back_inserter(new_idxs));
  return new_idxs;
}

template <class Container> Container tail(const Container &idxs, int start) {
  Container new_idxs;
  std::copy(idxs.begin() + start, idxs.end(), std::back_inserter(new_idxs));
  return new_idxs;
}
template <class I> std::vector<I> append(std::vector<I> idxs, I idx) {
  idxs.push_back(idx);
  return idxs;
}

template <class I> std::vector<I> append(I idx, std::vector<I> idxs) {
  std::vector<I> res{idx};
  std::copy(idxs.begin(), idxs.end(), std::back_inserter(res));
  return res;
}

template <class In, class F> auto vtransform(std::vector<In> in, F func) {
  std::vector<decltype(func(in[0]))> out;
  out.reserve(in.size());
  std::transform(in.begin(), in.end(), std::back_inserter(out), func);
  return out;
}

} // namespace slow
} // namespace hypercubes

#endif // UTILS_H_
