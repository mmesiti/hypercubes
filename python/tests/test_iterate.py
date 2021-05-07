#!/usr/bin/env python3
import partition_class_tree as pct
import partition_tree as pt
import partition_predicates as pp
import tree
import fixtures
import pytest


@pytest.mark.parametrize("idx", [
    (3, 1, 0, 1, 0),
    (0, 0, 4, 0, 0),
    (2, 1, 1, 0, 0),
    (1, 0, 2, 1, 0),
    (0, 1, 2, 1, 1),
])
def test_validate_idx_yes(idx):
    partitioning = fixtures.partitioning1D()
    assert pct.validate_idx(partitioning, idx)


@pytest.mark.parametrize("idx", [
    (3, 1, 0, 1, 1),
    (0, 0, 4, 0, -1),
    (0, 0, 5, 0, 0),
    (4, 1, 0, 1, 0),
    (5, -1, 0, 1, 0),
    (5, 1, 0, 1, 0),
    (3, 0, 3, 0, 0),
    (2, 1, 2, 0, 1),
    (1, 0, 1, 0, 0),
])
def test_validate_idx_no(idx):
    partitioning = fixtures.partitioning1D()
    assert not pct.validate_idx(partitioning, idx)


@pytest.fixture
def simple_size_tree():
    return (
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


def test_iterate_simple(simple_size_tree):
    idx = (0, 0, 0)
    idxp = (0, 0, 1)
    assert pt.iterate(simple_size_tree, idx) == idxp


def test_iterate_simple_carry(simple_size_tree):
    idx = (0, 0, 7)
    idxp = (0, 1, 0)
    assert pt.iterate(simple_size_tree, idx) == idxp


def test_iterate_simple_carry2(simple_size_tree):
    idx = (0, 1, 7)
    idxp = (1, 0, 0)
    assert pt.iterate(simple_size_tree, idx) == idxp


@pytest.mark.parametrize(
    "idx,idxp",
    [
        # Site 37   , Site 38
        ((3, 1, 0, 1, 0), (3, 1, 1, 0, 0)),
        # In this case the next site is physically BEFORE!
        # Site 6    , Site 5
        ((0, 0, 4, 0, 0), (0, 1, 0, 1, 0)),
        # In this case, the partition 2,1,1,1 is empty.
        # The next site is actually EVEN with this ordering,
        # Site 28   , Site 30
        ((2, 1, 1, 0, 0), (2, 1, 2, 0, 0)),
        # Site 13   , Site 15
        ((1, 0, 2, 1, 0), (1, 0, 2, 1, 1)),
        # Site 9,   , Site 10
        ((0, 1, 2, 1, 1), (0, 1, 3, 0, 0)),
    ])
def test_iterate(idx, idxp):
    partitioning = fixtures.partitioning1D()
    size_tree = pt.get_size_tree(partitioning, lambda _: True)
    assert pt.iterate(size_tree, idx) == idxp


def haloonly1D():
    partitioning = fixtures.partitioning1D()
    border_sites = fixtures.get_border_sites()

    # Ordering:
    # X1|X3|45|67|89
    # 1436587.. +3-1
    def reorder(arr):
        i = 1
        step = 0
        out = []
        while i < len(arr):
            out.append(arr[i])
            i += 3 if step % 2 == 0 else -1
            step += 1
        return out

    def get_halo_idx(bs):
        itwg = pct.get_indices_tree_with_ghosts(
            partitioning,  #
            (bs, ))
        return pct.get_relevant_indices_flat(itwg)

    # Skipping the first site
    return [
        (bs, tuple(i for i, _1, _2 in idx))  #
        for bs in reorder(border_sites)  #
        for halo_dim, idx in get_halo_idx(bs)  #
        if halo_dim
    ]


@pytest.mark.parametrize(
    "iidx,ipidxp",
    zip(
        haloonly1D()[:-1],  #
        haloonly1D()[1:]))
def test_iterate_size_tree_with_holes(iidx, ipidxp):
    i, idx = iidx
    ip, idxp = ipidxp
    partitioners = fixtures.partitioners_list_1D_42()
    partitioning = fixtures.partitioning1D()
    size_tree = pt.get_size_tree(
        partitioning, lambda idx: pp.no_bulk_borders(partitioners, idx))
    assert pt.iterate(size_tree, idx) == idxp
