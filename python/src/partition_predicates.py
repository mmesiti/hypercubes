#!/usr/bin/env python3
'''
A library of example predicates.
Every predicate should be written in a way
that it does not need to go
through the whole index,
A predicate takes a partitioners_list and an idx
(that may or may not be required to have the same length)
and returns a bool.

NOTE:
A predicate return true if the result cannot be determined
(e.g., if the index passed is too short).
This means that boolean logic does not work exactly
(One should have True, False, Maybe
and specify what to do in these three cases).
'''
from bool_maybe import BoolM


def only_NmD_halos(partitioners_list, idx, D):
    '''
    Returns true for partitions which have a dimension between
    N - D1 and N - D2 (both included )
    with D2 > D1.
    If we do not have enough hbb levels, return true
    (give the benefit of the doubt).
    '''
    def is_hbblevel(p):
        name, dirinfo, ptype, _ = p
        return ptype == "hbb"

    hbb_dimension = sum([is_hbblevel(p) for p in partitioners_list])
    halo_count = 0
    still_unknown = hbb_dimension
    for p, i in zip(partitioners_list, idx):
        if is_hbblevel(p):
            still_unknown -= 1
            halo_count += int(i in {0, 4})
            if D < halo_count:
                return BoolM.F
            elif still_unknown <= halo_count <= D - still_unknown:
                return BoolM.T

    return BoolM.M


def get_NmD_halo_predicate(D):
    return lambda p, i: only_NmD_halos(p, i, D)


def get_NmD1D2_halo_predicate(D1, D2):
    return lambda p, i: (
        only_NmD_halos(p, i, D2) & ~only_NmD(p,i,D1))

def no_bulk_borders(partitioners_list, idx):
    '''
    Selects only halos
    '''
    return (only_NmD_halos(partitioners_list,idx,1) & #
            ~ only_NmD_halos(partitioners_list,idx,0))

def only_specific_mpi_rank(partitioners_list, idx, MPI_ranks):
    def is_mpilevel(p):
        name, dirinfo, ptype, _ = p
        return "MPI" in name

    mpi_dimension = sum([is_mpilevel(p) for p in partitioners_list])
    MPI_partitioners_count = 0
    for p, i in zip(partitioners_list, idx):
        if is_mpilevel(p):
            if i != MPI_ranks[MPI_partitioners_count]:
                return BoolM.F
            MPI_partitioners_count += 1
            if MPI_partitioners_count == mpi_dimension:
                return BoolM.T

    return BoolM.M


def get_mpi_rank_predicate(MPI_ranks):
    return lambda p, i: only_specific_mpi_rank(p, i, MPI_ranks)
