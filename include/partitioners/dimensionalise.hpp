#ifndef __DIMENSIONALISE_H_
#define __DIMENSIONALISE_H_
#include "geometry/geometry.hpp"
#include "partitioning.hpp"
#include "utils/utils.hpp"
#include <algorithm>

namespace hypercubes {
namespace slow {
namespace partitioning {

// P1D is a Partitioning1D class
template <class P1D> class Dimensionalise : public IPartitioning {
public:
  template <class... Args>
  Dimensionalise(SizeParities sp_, int dimension_, std::string name_,
                 Args... other_args)
      : dimension(dimension_), spd(sp_),
        wrapped(spd[dimension], dimension, name_, other_args...){};
  Coordinates idx_to_coords(int idx, const Coordinates &offsets) const {
    return merge_in(wrapped.idx_to_coord(idx,                 //
                                         offsets[dimension]), //
                    offsets);                                 //
  }
  SizeParitiesD sub_sizeparity_info_list() const {
    auto res1D = wrapped.sub_sizeparity_info_list();
    return vtransform(res1D,
                      [this](SizeParity sp) { return merge_in(sp, spd); });
  };
  int idx_to_child_kind(int idx) const {
    return wrapped.idx_to_child_kind(idx);
  };
  int max_idx_value() const { return wrapped.max_idx_value(); };
  std::string get_name() const { return wrapped.get_name(); };
  std::string comments() const { return wrapped.comments(); };

  vector<IndexResultD> coord_to_idxs(const Coordinates &coord) const {
    vector<IndexResult> res1D = wrapped.coord_to_idxs(coord[dimension]);

    return vtransform(res1D, //
                      [this, &coord](IndexResult r1D) {
                        return IndexResultD{r1D.idx,                   //
                                            merge_in(r1D.rest, coord), //
                                            r1D.cached_flag};
                      });
  };

  const int dimension;
  int dimensionality() const { return spd.size(); };

private:
  const SizeParityD spd;
  P1D wrapped;

  template <class T, class Container>
  Container merge_in(T value, const Container iterable) const {
    auto res = iterable;
    res[dimension] = value;
    return res;
  }
};

} // namespace partitioning
} // namespace slow
} // namespace hypercubes

#endif // __DIMENSIONALISE_H_
