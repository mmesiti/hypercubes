#!/usr/bin/env python3
import fixtures
import partition_predicates as pps
import partition_class_tree as pct
import partition_tree as pt


def test_all_allocated_simple():
    partitioners_list = (
        ("MPI1", 0, "qper", 2),
        ("leaf1", 0, "leaf", None),
        ("END", None, "end", None),
    )

    partitioning = pct.get_partitioning(
        ((32, 0), ),
        partitioners_list,
    )

    size_tree = pt.get_allocation_sizes(partitioning, lambda _: True)

    print("Size tree:", size_tree)
    (n, _), _2 = size_tree

    assert n == 32
    assert size_tree == (
        (32, ()),
        (  #
            ((16, (0, )), ()),  #
            ((16, (1, )), ()),  #
        ))


def test_all_allocated_simple2():
    partitioners_list = (
        ("MPI1", 0, "qper", 2),
        ("VECTOR1", 0, "qper", 2),
        ("leaf1", 0, "leaf", None),
        ("END", None, "end", None),
    )

    partitioning = pct.get_partitioning(
        ((32, 0), ),
        partitioners_list,
    )

    size_tree = pt.get_allocation_sizes(partitioning, lambda _: True)

    print("Size tree:", size_tree)
    (n, _), _2 = size_tree

    assert n == 32
    assert size_tree == (
        (32, ()),
        (  #
            ((16, (0, )), (
                ((8, (0, 0)), ()),
                ((8, (0, 1)), ()),
            )),  #
            ((16, (1, )), (
                ((8, (1, 0)), ()),
                ((8, (1, 1)), ()),
            )),  #
        ))


def test_all_allocated():
    partitioners_list = fixtures.partitioners_list_1D_42()

    partitioning = pct.get_partitioning(
        ((42, 0), ),
        partitioners_list,
    )

    size_tree = pt.get_allocation_sizes(partitioning, lambda _: True)

    (n, _), _2 = size_tree

    assert n == 42 + 2 * 2 * 4  # halos for 4 MPI partition having 2 sub partitions


def test_only_bb_allocated():
    partitioners_list = fixtures.partitioners_list_1D_42()

    partitioning = pct.get_partitioning(
        ((42, 0), ),
        partitioners_list,
    )

    def predicate(i):
        return pps.get_NmD_predicate(0)(partitioners_list, i)

    size_tree = pt.get_allocation_sizes(partitioning, predicate)

    (n, _), _2 = size_tree

    assert n == 42


def test_single_mpi_rank():
    partitioners_list = fixtures.partitioners_list_1D_42()

    partitioning = pct.get_partitioning(
        ((42, 0), ),
        partitioners_list,
    )

    def predicate(idx):
        return (pps.only_specific_mpi_rank(partitioners_list,idx,(3,))
                and pps.get_NmD_predicate(0)(partitioners_list, idx))

    size_tree = pt.get_allocation_sizes(partitioning, predicate)

    (n, _), _2 = size_tree

    assert n == 9


def test_single_mpi_rank_4D_bulk():
    partitioners_list = fixtures.partitioners_list_4D_42()

    partitioning = pct.get_partitioning(
        ((42, 0), )*4,
        partitioners_list,
    )

    def predicate(idx):
        MPI_Rank=(3,3,3,3)
        return (pps.only_specific_mpi_rank(partitioners_list,idx,MPI_Rank)
                and pps.get_NmD_predicate(0)(partitioners_list,idx))

    size_tree = pt.get_allocation_sizes(partitioning, predicate)

    (n, _), _2 = size_tree

    assert n == 9**4


def test_single_mpi_rank_4D_bulkand3Dhalo():
    partitioners_list = fixtures.partitioners_list_4D_42()

    partitioning = pct.get_partitioning(
        ((42, 0), )*4,
        partitioners_list,
    )

    def predicate(idx):
        MPI_Rank=(3,3,3,3)
        return (pps.only_specific_mpi_rank(partitioners_list,idx,MPI_Rank)
                and pps.get_NmD_predicate(6)(partitioners_list,idx))

    size_tree = pt.get_allocation_sizes(partitioning, predicate)

    (n, _), _2 = size_tree

    assert n == 11**4 #9**4+2*4*9**3
