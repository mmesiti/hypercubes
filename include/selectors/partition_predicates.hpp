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
#include "geometry/geometry.hpp"
#include "partitioners/partitioners.hpp"
#include <functional>

namespace hypercubes {
namespace slow {

// base predicates

namespace selectors {

BoolM halos_upto_NmD(Indices idxs, PartList partitioners, int D);
BoolM mpi_rank(Indices idxs, PartList partitioners, vector<int> cart_mpi_ranks);
BoolM no_bulk_borders(Indices idxs, PartList partitioners);
BoolM hbb_slice(Indices idxs, PartList partitioners, int direction,
                int hbb_idx);

} // namespace selectors
using PartitionPredicate = std::function<BoolM(Indices)>;
// Generic function that returns predicates
template <class... Args>
PartitionPredicate getp(BoolM (*predicate)(Indices, Args...), Args... args) {
  return
      [predicate, args...](Indices idxs) { return predicate(idxs, args...); };
}

// Ways to compose predicates.
PartitionPredicate operator&&(PartitionPredicate, PartitionPredicate);
PartitionPredicate operator||(PartitionPredicate, PartitionPredicate);
PartitionPredicate operator!(PartitionPredicate);

} // namespace slow
} // namespace hypercubes

#endif // PARTITION_PREDICATES_H_
