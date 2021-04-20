#ifndef UTILS_H_
#define UTILS_H_
#include <tuple>
#include <vector>

template <int N> struct TupleCutter {
  template <typename Tuple> static auto cut(Tuple t) {
    auto trivial_selector = [](auto t){return t;};
    return cut(t,trivial_selector);
  }

  template <typename Tuple, typename Selector> static auto cut(Tuple t, Selector s) {
    auto head = std::make_tuple(s(std::get<N>(t)));
    auto tail = TupleCutter<N - 1>::cut(t,s);
    return std::tuple_cat(tail, head);
  }
};

template <> struct TupleCutter<0> {

  template <typename Tuple> static auto cut(Tuple t) {
    auto trivial_selector = [](auto t){return t;};
    return cut(t,trivial_selector);
  }

  template <typename Tuple, typename Selector> static auto cut(Tuple t, Selector s) {
    return std::make_tuple(s(std::get<0>(t)));
  }
};

// template <typename T, typename... TS>
// std::tuple<T, TS...> select(int i,
//                            std::tuple<std::vector<T>, std::vector<TS...>> vs)
//                            {
//  auto head = std::make_tuple<T>(std::get<0>(vs)[i]);
//  auto rest = select(i, );k
//}
//
///**
// * Zips vectors
// * */
// template <typename T, typename... TS> class zipv {
//
// public:
//  using VTuple = std::tuple<std::vector<T>, std::vector<TS>...>;
//  using Tuple = std::tuple<T, TS...>;
//
//  zipv(const std::vector<T> &v, const std::vector<TS> &...vs) {
//    _vs = std::make_tuple(v, vs...);
//  }
//
//  auto operator[](int i) {
//    std::tuple<T, TS...> res;
//    fill_res<TS>(res, _vs, i);
//    return res;
//  }
//
//  auto begin(){};
//  auto end(){};
//
// private:
//  VTuple _vs;
//};

#endif // UTILS_H_
