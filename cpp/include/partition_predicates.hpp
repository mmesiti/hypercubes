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

#include "bool_maybe.hpp"
#include "partition_class_tree.hpp"
#include <functional>

namespace hypercubes {
namespace slow {

using PartitionPredicate = std::function<BoolM(Indices)>;

BoolM only_NmD_halos(PartList partitioners, Indices idxs, int D);
PartitionPredicate get_NmD_halo_predicate(PartList partitioners, int D);

BoolM only_specific_mpi_rank(PartList partitioners, Indices idxs,
                             vector<int> MPI_ranks);

PartitionPredicate get_mpi_rank_predicate(PartList partitioners,
                                          vector<int> MPI_ranks);

BoolM no_bulk_borders(PartList partitioners, Indices idx);

} // namespace slow
} // namespace hypercubes

#endif // PARTITION_PREDICATES_H_
