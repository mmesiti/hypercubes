#include "partition_predicates.hpp"
namespace hypercubes {
namespace slow {
BoolM only_NmD_halos(PartList partitioners, Indices idxs, int D) {
  int halo_count = 0;
  for (int i = 0; i < idxs.size(); ++i) {
    bool is_hbb_level =
        partitioners[i]->get_name().find("Halo") != std::string::npos;
    bool is_halo = idxs[i] == 0 or idxs[i] == 4;
    if (is_hbb_level and is_halo)
      ++halo_count;
    if (halo_count > D)
      return BoolM::F;
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

  int MPI_partitioners_count = 0;
  for (int i = 0; i < idxs.size(); ++i) {
    if (partitioners[i]->get_name().find("MPI") != std::string::npos) {
      if (idxs[i] != MPI_ranks[MPI_partitioners_count])
        return BoolM::F;
      ++MPI_partitioners_count;
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
  return only_NmD_halos(partitioners, idx, 1) and
         not only_NmD_halos(partitioners, idx, 0);
}
} // namespace slow
} // namespace hypercubes
