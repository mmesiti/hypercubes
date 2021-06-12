#include "selectors/partition_predicates.hpp"
#include "partitioners/partitioners.hpp"
#include <memory>
namespace hypercubes {
namespace slow {

namespace predicates_detail {

bool is_hbb(IPartRP p) {
  return bool(std::dynamic_pointer_cast<partitioners::HBB>(p));
};
} // namespace predicates_detail
// base predicates
BoolM halos_up_to_NmD(Indices idxs, PartList partitioners, int D) {

  using predicates_detail::is_hbb;
  int hbb_dimension = std::count_if(partitioners.begin(), //
                                    partitioners.end(),   //
                                    is_hbb);
  int halo_count = 0;
  int still_unknown = hbb_dimension;
  for (int i = 0; i < idxs.size(); ++i) {
    if (is_hbb(partitioners[i])) {
      still_unknown--;
      bool is_halo = idxs[i] == 0 or idxs[i] == 4;
      if (is_halo)
        ++halo_count;
      if (D < halo_count) {
        return BoolM::F;
      } else if (still_unknown <= halo_count and //
                 halo_count <= D - still_unknown) {
        return BoolM::T;
      }
    }
  }
  return BoolM::M;
}
BoolM mpi_rank(Indices idxs, PartList partitioners,
               vector<int> cart_mpi_ranks) {

  auto is_mpilevel = [](auto p) {
    return p->get_name().find("MPI") != std::string::npos;
  };
  int mpi_dimension = std::count_if(partitioners.begin(), //
                                    partitioners.end(),   //
                                    is_mpilevel);
  int MPI_partitioners_count = 0;
  for (int i = 0; i < idxs.size(); ++i) {
    if (is_mpilevel(partitioners[i])) {
      if (idxs[i] != cart_mpi_ranks[MPI_partitioners_count])
        return BoolM::F;
      ++MPI_partitioners_count;
      if (MPI_partitioners_count == mpi_dimension)
        return BoolM::T;
    }
  }

  return BoolM::M;
}
BoolM no_bulk_borders(Indices idxs, PartList partitioners) {
  BoolM in_halo1D = halos_up_to_NmD(idxs, partitioners, 1);
  BoolM in_bb = halos_up_to_NmD(idxs, partitioners, 0);
  return in_halo1D and not in_bb;
}
BoolM hbb_slice(Indices idxs, PartList partitioners, int direction,
                int hbb_idx) {
  using predicates_detail::is_hbb;
  for (int i = 0; i < idxs.size(); ++i) {
    if (is_hbb(partitioners[i]) and
        std::dynamic_pointer_cast<partitioners::HBB>(partitioners[i])
                ->get_dimension() == direction) {

      if (idxs[i] == hbb_idx)
        return BoolM::T;
      else
        return BoolM::F;
    }
  }

  return BoolM::M;
}

// Ways to compose predicates.
PartitionPredicate operator!(PartitionPredicate p) {
  return [p = std::move(p)](Indices idxs) -> BoolM { return not p(idxs); };
}
PartitionPredicate operator&&(PartitionPredicate a, PartitionPredicate b) {

  return [a = std::move(a), b = std::move(b)](Indices idxs) -> BoolM {
    return a(idxs) and b(idxs);
  };
}
PartitionPredicate operator||(PartitionPredicate a, PartitionPredicate b) {

  return [a = std::move(a), b = std::move(b)](Indices idxs) -> BoolM {
    return a(idxs) or b(idxs);
  };
}

} // namespace slow
} // namespace hypercubes
