#ifndef SITE_H_
#define SITE_H_
#include "partitioning.hpp"

namespace hypercubes {
namespace slow {
namespace partitioning {

/** An object of this type
 *represents a leaf in the partition tree.*/
class Site : public IPartitioning {
public:
  Site(const PartInfoD &sp_);
  Coordinates idx_to_coords(int idx, const Coordinates &offset) const;
  Sizes idx_to_sizes(int idx) const;
  PartInfosD sub_partinfo_kinds() const;
  int idx_to_partinfo_kind(int idx) const;
  int max_idx_value() const;
  bool is_leaf() const;
  std::string get_name() const;
  std::string comments() const;
  vector<IndexResultD> coord_to_idxs(const Coordinates &coord) const;
  int dimensionality() const;

private:
  int dimension;
  void check_idx(int idx) const;
};

} // namespace partitioning
} // namespace slow
} // namespace hypercubes

#endif // SITE_H_
