#ifndef __TUPLE_PRINTER_H_
#define __TUPLE_PRINTER_H_
#include "print_utils.hpp"
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace help {

template <int N> struct TuplePrinter {
  template <class... TS> static std::string tuple_to_str(std::tuple<TS...> t) {
    std::stringstream ss;
    const int FieldToPrint = sizeof...(TS) - N;
    ss << std::get<FieldToPrint>(t) << ", "
       << TuplePrinter<N - 1>::template tuple_to_str<TS...>(t);
    return ss.str();
  }
};

template <> struct TuplePrinter<1> {
  template <class... TS> static std::string tuple_to_str(std::tuple<TS...> t) {
    std::stringstream ss;
    ss << std::get<sizeof...(TS) - 1>(t);
    return ss.str();
  }
};

} // namespace help

template <class... TS> std::string tuple_to_str(std::tuple<TS...> t) {
  return std::string("(") + help::TuplePrinter<sizeof...(TS)>::tuple_to_str(t) +
         std::string(")");
}

#endif // __TUPLE_PRINTER_H_
