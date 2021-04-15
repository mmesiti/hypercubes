#!/usr/bin/env python3
import fixtures
import partition_predicates as pps
import partition_class_tree as pct
import partition_tree as pt
import tree

import itertools as it
import math
import pytest


@pytest.fixture
def simple_size_tree():
    partitioners_list = (
        ("MPI1", 0, "qper", 2),
        ("leaf1", 0, "leaf", None),
        ("END", None, "end", None),
    )

    partitioning = pct.get_partitioning(
        ((32, 0), ),
        partitioners_list,
    )

    return pt.get_size_tree(partitioning, lambda _: True)


def test_all_allocated_simple(simple_size_tree):

    print("Size tree:", simple_size_tree)
    (n, _), _2 = simple_size_tree

    assert n == 32
    assert simple_size_tree == (
        (32, None),
        (  #
            ((16, 0), ()),  #
            ((16, 1), ()),  #
        ))


@pytest.fixture
def less_simple_size_tree():
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

    return pt.get_size_tree(partitioning, lambda _: True)


def test_all_allocated_simple2(less_simple_size_tree):

    print("Size tree:", less_simple_size_tree)
    (n, _), _2 = less_simple_size_tree

    assert n == 32
    assert less_simple_size_tree == (
        (32, None),
        (  #
            ((16, 0), (
                ((8, 0), ()),
                ((8, 1), ()),
            )),  #
            ((16, 1), (
                ((8, 0), ()),
                ((8, 1), ()),
            )),  #
        ))


def test_all_allocated():
    partitioners_list = fixtures.partitioners_list_1D_42()

    partitioning = pct.get_partitioning(
        ((42, 0), ),
        partitioners_list,
    )

    size_tree = pt.get_size_tree(partitioning, lambda _: True)

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

    size_tree = pt.get_size_tree(partitioning, predicate)

    (n, _), _2 = size_tree

    assert n == 42


def test_single_mpi_rank():
    partitioners_list = fixtures.partitioners_list_1D_42()

    partitioning = pct.get_partitioning(
        ((42, 0), ),
        partitioners_list,
    )

    def predicate(idx):
        return (pps.only_specific_mpi_rank(partitioners_list, idx, (3, ))
                and pps.get_NmD_predicate(0)(partitioners_list, idx))

    size_tree = pt.get_size_tree(partitioning, predicate)

    (n, _), _2 = size_tree

    assert n == 9


def test_single_mpi_rank_4D_bulk():
    partitioners_list = fixtures.partitioners_list_4D_42()

    partitioning = pct.get_partitioning(
        ((42, 0), ) * 4,
        partitioners_list,
    )

    def predicate(idx):
        MPI_Rank = (3, 3, 3, 3)
        return (pps.only_specific_mpi_rank(partitioners_list, idx, MPI_Rank)
                and pps.get_NmD_predicate(0)(partitioners_list, idx))

    size_tree = pt.get_size_tree(partitioning, predicate)

    (n, _), _2 = size_tree

    assert n == 9**4


def test_single_mpi_rank_4D_bulkand3Dhalo():
    partitioners_list = fixtures.partitioners_list_4D_42()

    partitioning = pct.get_partitioning(
        ((42, 0), ) * 4,
        partitioners_list,
    )

    def predicate(idx):
        MPI_Rank = (3, 3, 3, 3)
        return (pps.only_specific_mpi_rank(partitioners_list, idx, MPI_Rank)
                and pps.get_NmD_predicate(1)(partitioners_list, idx))

    size_tree = pt.get_size_tree(partitioning, predicate)

    (n, _), _2 = size_tree

    def cube_halo1_vol(dimensions):
        vn = math.prod(dimensions)
        return vn + sum(2 * vn / d for d in dimensions)

    subsizes = ((4, 5), ) * 4  #

    assert n == sum(cube_halo1_vol(ds) for ds in it.product(*subsizes))


def test_single_mpi_rank_4D_bulkand3Dhalo_simpler():
    partitioners_list = (
        ("MPI X", 0, "qper", 4),
        ("MPI Y", 1, "qper", 4),
        ("MPI Z", 2, "qper", 4),
        ("MPI T", 3, "qper", 4),
        ("halos X", 0, "hbb", 1),
        ("halos Y", 1, "hbb", 1),
        ("halos Z", 2, "hbb", 1),
        ("halos T", 3, "hbb", 1),
        ("EO", (True, ) * 4, "eo", None),
        ("EO-flattened", 4, "leaf", None),
        ("END", None, "end", None),
    )

    partitioning = pct.get_partitioning(
        ((42, 0), ) * 4,
        partitioners_list,
    )

    def predicate(idx):
        MPI_Rank = (3, 3, 3, 3)
        return (pps.only_specific_mpi_rank(partitioners_list, idx, MPI_Rank)
                and pps.get_NmD_predicate(4)(partitioners_list, idx))

    size_tree = pt.get_size_tree(partitioning, predicate)

    (n, _), _2 = size_tree

    assert n == 11**4


def test_start_tree_simple(simple_size_tree):
    start_tree = pt.get_start_tree(simple_size_tree)

    assert start_tree == (
        (0, None),
        (  #
            ((0, 0), ()),  #
            ((16, 1), ()),  #
        ))


def test_start_tree_simple2(less_simple_size_tree):
    start_tree = pt.get_start_tree(less_simple_size_tree)

    assert start_tree == (
        (0, None),
        (  #
            ((0, 0), (
                ((0, 0), ()),
                ((8, 1), ()),
            )),  #
            ((16, 1), (
                ((16, 0), ()),
                ((24, 1), ()),
            )),  #
        ))


def test_no_zero_size_partitions():
    '''
    The size_tree does not contain zero-size partitions.
    This property makes it easier to iterate on.

    '''
    partitioners_list = fixtures.partitioners_list_1D_42()
    partitioning = pct.get_partitioning(
        ((42, 0), ),
        partitioners_list,
    )
    size_tree = pt.get_size_tree(partitioning, lambda i: True)

    sizes_list = (s for s, _ in tree.depth_first_flatten(size_tree))
    assert 0 not in sizes_list
