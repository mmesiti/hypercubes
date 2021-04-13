#!/usr/bin/env python3
import partitioning_tree as pt
import fixtures
from fixtures import nexamples
import tree

import pytest
from hypothesis import given, settings, HealthCheck
from hypothesis import strategies as st


@pytest.fixture
def partitioning4Dfixture():
    return fixtures.partitioning4D()


@pytest.fixture
def partitioning4Dnoeo():
    sizes = (42, 42, 42, 42)

    geom_infos = tuple((s, 0) for s in sizes)

    X, Y, Z, T = list(range(4))

    partitioners_list = (
        ("MPI X", X, "qper", 4),
        ("MPI Y", Y, "qper", 4),
        ("MPI Z", Z, "qper", 4),
        ("MPI T", T, "qper", 4),
        ("VECTOR X", X, "qopen", 2),
        ("VECTOR Y", Y, "qopen", 2),
        ("VECTOR Z", Z, "qopen", 2),
        ("VECTOR T", T, "qopen", 2),
        ("halos X", X, "hbb", 1),
        ("halos Y", Y, "hbb", 1),
        ("halos Z", Z, "hbb", 1),
        ("halos T", T, "hbb", 1),
        ("Leaf X", X, "leaf", None),
        ("Leaf Y", Y, "leaf", None),
        ("Leaf Z", Z, "leaf", None),
        ("Leaf T", T, "leaf", None),
    )

    return pt.get_partitioning(geom_infos, partitioners_list)


@settings(max_examples=nexamples(1000),
          suppress_health_check=[HealthCheck.function_scoped_fixture])
@given(xs=st.lists(st.integers(min_value=0, max_value=41),
                   min_size=4,
                   max_size=4))
def test_idx_to_coords_noeo(xs, partitioning4Dnoeo):
    xst = tuple(xs)
    it = pt.get_indices_tree(partitioning4Dnoeo, xst)
    idxs = tree.get_all_paths(it)
    for idx in idxs:
        res = pt.get_coord_from_idx(partitioning4Dnoeo, idx, 4)
        assert res == xst


@pytest.mark.parametrize("xs", [
    (33, ),
    (3, ),
    (3, ),
    (0, ),
    (8, ),
    (0, ),
    (10, ),
])
def test_idx_to_coords1D(xs):
    sizes = (42, )
    geom_infos = tuple((s, 0) for s in sizes)

    X, EXTRA = list(range(2))

    partitioners_list = (
        ("MPI X", X, "qper", 2),
        ("EO", tuple(True for _ in sizes), "eo", None),
        ("EO-flattened", EXTRA, "leaf", None),
    )

    partitioning = pt.get_partitioning(geom_infos, partitioners_list)

    xst = tuple(xs)
    it = pt.get_indices_tree(partitioning, xst)
    idxs = tree.get_all_paths(it)
    for idx in idxs:
        res = pt.get_coord_from_idx(partitioning, idx, 2)
        assert res == xst


@pytest.mark.parametrize("xs", [
    (15, 33),
    (25, 3),
    (41, 3),
    (23, 0),
    (33, 8),
    (21, 0),
    (22, 10),
])
def test_idx_to_coords_full(xs):
    sizes = (44, 42)
    geom_infos = tuple((s, 0) for s in sizes)

    X, Y, EXTRA = list(range(3))

    partitioners_list = (
        ("MPI X", X, "qper", 2),
        ("MPI Y", Y, "qper", 2),
        ("EO", tuple(True for _ in sizes), "eo", None),
        ("EO-flattened", EXTRA, "leaf", None),
    )

    partitioning = pt.get_partitioning(geom_infos, partitioners_list)

    xst = tuple(xs)
    it = pt.get_indices_tree(partitioning, xst)
    idxs = tree.get_all_paths(it)
    for idx in idxs:
        res = pt.get_coord_from_idx(partitioning, idx, 3)
        assert res == xst