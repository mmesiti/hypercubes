#!/usr/bin/env python3
import partition_class_tree as pct
import pytest


@pytest.mark.parametrize("idx,expsizes", [
    ((0, ), (21, )),
    ((1, ), (21, )),
    ((1, 0), (11, )),
    ((1, 1), (10, )),
    ((1, 1, 0), (1, )),
    ((1, 1, 1), (1, )),
    ((1, 1, 2), (8, )),
])
def test_get_sizes_from_idx_1D(idx, expsizes):
    sizes = (42, )
    geom_infos = tuple((s, 0) for s in sizes)

    X = 0

    partitioners_list = (
        ("MPI X", X, "qper", 2),
        ("VECTOR X", X, "qopen", 2),
        ("HBB X", X, "hbb", 1),
        ("Leaf X", X, "leaf", None),
        ("END", None, "end", None),
    )

    partitioning = pct.get_partition_class_tree(geom_infos, partitioners_list)
    print(pct.partition_class_tree_to_str(partitioning, '', 10))

    assert expsizes == pct.get_sizes_from_idx(partitioning, idx, sizes)


@pytest.mark.parametrize("idx,expsizes", [
    ((0, ), (21, 42)),
    ((0, 0), (21, 21)),
    ((0, 0, 0), (11, 21)),
    ((0, 0, 0, 0), (11, 11)),
    ((1, 1, 0, 0), (11, 11)),
    ((1, 0, 1, 0), (10, 11)),
    ((1, 0, 1, 0, 0, 0), (1, 1)),
])
def test_get_sizes_from_idx_2D(idx, expsizes):
    sizes = (42, 42)
    geom_infos = tuple((s, 0) for s in sizes)

    X, Y = 0, 1

    partitioners_list = (
        ("MPI X", X, "qper", 2),
        ("MPI Y", Y, "qper", 2),
        ("VECTOR X", X, "qopen", 2),
        ("VECTOR Y", Y, "qopen", 2),
        ("HBB X", X, "hbb", 1),
        ("HBB Y", Y, "hbb", 1),
        ("Leaf X", X, "leaf", None),
        ("Leaf Y", Y, "leaf", None),
        ("END", None, "end", None),
    )

    partitioning = pct.get_partition_class_tree(geom_infos, partitioners_list)

    assert expsizes == pct.get_sizes_from_idx(partitioning, idx, sizes)


@pytest.mark.parametrize("idx,starts_ends", [
    ((0, ), ((0, 16), )),
    ((1, ), ((16, 32), )),
    ((0, 0), ((0, 8), )),
    ((0, 1), ((8, 16), )),
    ((1, 0), ((16, 24), )),
    ((1, 1), ((24, 32), )),
])
def test_partition_limits_1D_simple(idx, starts_ends):
    partitioners_list = (
        ("MPI1", 0, "qper", 2),
        ("VECTOR1", 0, "qper", 2),
        ("leaf1", 0, "leaf", None),
        ("END", None, "end", None),
    )

    geom_info = ((32, 0), )
    partition_class_tree = pct.get_partition_class_tree(
        geom_info, partitioners_list)

    assert pct.get_partition_limits(partition_class_tree, idx,
                                    (32, )) == starts_ends


@pytest.mark.parametrize("idx,starts_ends", [
    ((0, ), ((0, 21), (0, 42))),
    ((0, 0), ((0, 21), (0, 21))),
    ((0, 0, 0, 0), ((0, 11), (0, 11))),
    ((1, 0, 1, 1), ((32, 42), (11, 21))),
    ((1, 1, 0, 1), ((21, 32), (32, 42))),
    ((1, 1, 0, 1, 0), ((20, 21), (32, 42))),
    ((1, 1, 0, 1, 0, 2), ((20, 21), (33, 41))),
])
def test_get_partition_limits_from_idx_2D(idx, starts_ends):
    sizes = (42, 42)
    geom_infos = tuple((s, 0) for s in sizes)

    X, Y = 0, 1

    partitioners_list = (
        ("MPI X", X, "qper", 2),
        ("MPI Y", Y, "qper", 2),
        ("VECTOR X", X, "qopen", 2),
        ("VECTOR Y", Y, "qopen", 2),
        ("HBB X", X, "hbb", 1),
        ("HBB Y", Y, "hbb", 1),
        ("Leaf X", X, "leaf", None),
        ("Leaf Y", Y, "leaf", None),
        ("END", None, "end", None),
    )

    partition_class_tree = pct.get_partition_class_tree(
        geom_infos, partitioners_list)

    assert pct.get_partition_limits(partition_class_tree, idx,
                                    (42, 42)) == starts_ends
