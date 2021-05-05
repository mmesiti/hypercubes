#ifndef PARTITION_PREDICATES_H_
#define PARTITION_PREDICATES_H_
/**
 * A library of example predicates.
 * Evety predicate should be written in a way
 * that it does not need to go
 * through the whole index.
 * A predicate takes a list of partitioners
 * and an idx
 * (that may or may not be required to have the same length)
 * and returns a bool.
 * The selection might be reliant
 * on the name of the levels/partitioners.
 * */

#include "partition_class_tree.hpp"
#include <functional>

namespace hypercubes {
namespace slow {
bool only_NmD_halos(PartList partitioners, Indices idxs, int D);
std::function<bool(PartList, Indices)> get_NmD_halo_predicate(int D);

bool only_specific_mpi_rank(PartList partitioners, Indices idxs,
                            vector<int> MPI_ranks);

std::function<bool(PartList, Indices)>
get_mpi_rank_predicate(vector<int> MPI_ranks);

} // namespace slow
} // namespace hypercubes

#endif // PARTITION_PREDICATES_H_
