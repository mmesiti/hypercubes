#include "selectors/partition_predicates.hpp"
#include "partitioners/partitioners.hpp"
#include <memory>
namespace hypercubes {
namespace slow {
bool is_hbb(IPartRP p) {
  return bool(std::dynamic_pointer_cast<partitioners::HBB>(p));
};

BoolM only_NmD_halos(PartList partitioners, Indices idxs, int D) {

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
      if (D < halo_count)
        return BoolM::F;
      else if (still_unknown <= halo_count and //
               halo_count <= D - still_unknown)
        return BoolM::T;
    }
  }
  return BoolM::M;
}

PartitionPredicate get_NmD_halo_predicate(PartList partitioners, int D) {
  return [D, partitioners](Indices idxs) -> BoolM {
    return only_NmD_halos(partitioners, idxs, D);
  };
}

BoolM only_specific_mpi_rank(PartList partitioners, Indices idxs,
                             vector<int> MPI_ranks) {

  auto is_mpilevel = [](auto p) {
    return p->get_name().find("MPI") != std::string::npos;
  };
  int mpi_dimension = std::count_if(partitioners.begin(), //
                                    partitioners.end(),   //
                                    is_mpilevel);
  int MPI_partitioners_count = 0;
  for (int i = 0; i < idxs.size(); ++i) {
    if (is_mpilevel(partitioners[i])) {
      if (idxs[i] != MPI_ranks[MPI_partitioners_count])
        return BoolM::F;
      ++MPI_partitioners_count;
      if (MPI_partitioners_count == mpi_dimension)
        return BoolM::T;
    }
  }

  return BoolM::M;
}

PartitionPredicate get_mpi_rank_predicate(PartList partitioners,
                                          vector<int> MPI_ranks) {
  return [MPI_ranks, partitioners](Indices idxs) -> BoolM {
    return only_specific_mpi_rank(partitioners, idxs, MPI_ranks);
  };
}

BoolM no_bulk_borders(PartList partitioners, Indices idx) {
  BoolM in_halo1D = only_NmD_halos(partitioners, idx, 1);
  BoolM in_bb = only_NmD_halos(partitioners, idx, 0);
  return in_halo1D and not in_bb;
}

PartitionPredicate get_hbb_slice_predicate(PartList partitioners, int direction,
                                           int hbb_idx) {

  return [partitioners, direction, hbb_idx](Indices idx) -> BoolM {
    return hbb_slice(partitioners, idx, direction, hbb_idx);
  };
}

BoolM hbb_slice(PartList partitioners, Indices idxs, int direction,
                int hbb_idx) {
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

} // namespace slow
} // namespace hypercubes
