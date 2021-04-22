#ifndef PARTITIONING1D_H_
#define PARTITIONING1D_H_
#include "geometry.hpp"
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace hypercubes {
namespace slow {
class Partitioning1D {
public:
  Partitioning1D(SizeParity sp, int dimension_, std::string name_)
      : size(sp.size), parity(sp.parity), dimension(dimension_), name(name_) {}
  int start(int idx) const;
  int end(int idx) const;
  std::vector<SizeParity> sub_sizeparity_info_list() const;
  int idx_to_child_kind(int idx) const;
  int idx_to_size(int idx) const;
  virtual int max_idx_value() const = 0;
  virtual std::vector<IndexResult> coord_to_idxs(int relative_x) const = 0;
  virtual int idx_to_coord(int idx, int offset) const = 0;
  virtual std::string comments() const = 0;

protected:
  int size;
  Parity parity;
  int dimension;
  std::string name;

  Parity idx_to_start_parity(int idx) const;
  std::vector<int> limits;
};

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
         std::string(" )");
}

} // namespace slow
} // namespace hypercubes

#endif // PARTITIONING1D_H_
