#!/usr/bin/env python3
'''
A library of example predicates.
Every predicate should be written in a way
that it does not need to go
through the whole index.
A predicate takes a partitioners_list and an idx
(that may be required to have the same length)
and returns a bool.
'''


def only_NmD_halos(partitioners_list, idx, D):
    halo_count = 0
    for p, i in zip(partitioners_list, idx):
        name, dirinfo, ptype, _ = p
        if ptype == "hbb" and i in {0, 4}:
            halo_count += 1
            if halo_count > D:
                return False
    return True


def get_NmD_halo_predicate(D):
    return lambda p, i: only_NmD_halos(p, i, D)


def only_specific_mpi_rank(partitioners_list, idx, MPI_ranks):
    MPI_partitioners_count = 0
    for p, i in zip(partitioners_list, idx):
        name, dirinfo, ptype, _ = p
        if "MPI" in name:
            if i != MPI_ranks[MPI_partitioners_count]:
                return False
            MPI_partitioners_count += 1
    return True


def get_mpi_rank_predicate(MPI_ranks):
    return lambda p, i: only_specific_mpi_rank(p, i, MPI_ranks)
