#ifndef __DIMENSIONALISE_H_
#define __DIMENSIONALISE_H_
#include "partitioning.hpp"
#include <algorithm>

namespace hypercubes {
namespace slow {

// P1D is a Partitioning1D class
template <class P1D> class Dimensionalise : public IPartitioning {
public:
  template <class... Args>
  Dimensionalise(SizeParities sp_, int dimension_, std::string name_,
                 Args... other_args)
      : dimension(dimension_), spd(sp_), name(name_),
        wrapped(spd[dimension], dimension, name, other_args...){};
  Coordinates idx_to_coords(int idx, const Coordinates &offsets) const {
    return merge_in(wrapped.idx_to_coord(idx,                 //
                                         offsets[dimension]), //
                    offsets);                                 //
  }
  Sizes idx_to_sizes(int idx, const Sizes &sizes) const {
    return merge_in(wrapped.idx_to_size(idx), //
                    sizes);                   //
  };
  SizeParitiesD sub_sizeparity_info_list() const {
    SizeParitiesD res;
    auto res1D = wrapped.sub_sizeparity_info_list();
    std::transform(res1D.begin(), res1D.end(), std::back_inserter(res),
                   [this](SizeParity sp) { return merge_in(sp, spd); });
    return res;
  };
  int idx_to_child_kind(int idx) const {
    return wrapped.idx_to_child_kind(idx);
  };
  int max_idx_value() const { return wrapped.max_idx_value(); };
  std::string comments() const { return wrapped.comments(); };

  vector<IndexResultD> coord_to_idxs(const Coordinates &coord) const {
    vector<IndexResultD> res;
    vector<IndexResult> res1D = wrapped.coord_to_idxs(coord[dimension]);

    std::transform(res1D.begin(), res1D.end(), std::back_inserter(res),
                   [this, &coord](IndexResult r1D) {
                     return IndexResultD{r1D.idx, merge_in(r1D.rest, coord),
                                         r1D.cached_flag};
                   });
    return res;
  };

  const int dimension;

private:
  const SizeParityD spd;
  const std::string name;
  P1D wrapped;

  template <class T, class Container>
  Container merge_in(T value, const Container iterable) const {
    auto res = iterable;
    res[dimension] = value;
    return res;
  }
};

} // namespace slow
} // namespace hypercubes

#endif // __DIMENSIONALISE_H_
