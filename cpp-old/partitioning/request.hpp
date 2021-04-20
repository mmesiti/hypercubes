#ifndef __REQUEST_H_
#define __REQUEST_H_

#include <array>
#include <vector>

namespace hypercubes {
namespace slow {

template <int Dimensionality> struct PartitioningRequest {
  using SizeDefinition = std::array<int, Dimensionality>;

  virtual std::vector<SizeDefinition> split_geometry(SizeDefinition) = 0;
};

template <int Dimensionality, int Dimension>
struct PR1D : PartitioningRequest<Dimensionality> {};

template <int Dimensionality>
struct EOPR : PartitioningRequest<Dimensionality> {
  using CBFlags = std::array<bool, Dimensionality>;
  using SizeDefinition =
      typename PartitioningRequest<Dimensionality>::SizeDefinition;
  int split_dir;
  CBFlags cbflags;

  EOPR(int split_dir_, const CBFlags &cbflags_)
      : split_dir(split_dir_), cbflags(cbflags_){};

  auto split_geometry(SizeDefinition s) {
    auto res = s;
    s[split_dir] /= 2;
    return std::vector<SizeDefinition>{res};
  };
};

} // namespace slow

} // namespace hypercubes

#endif // __REQUEST_H_
