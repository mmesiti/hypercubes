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

#TODO:
# add case "with holes", e.g., with predicate != lambda _ : True


def haloonly1D():
    partitioning = fixtures.partitioning1D()
    border_sites = fixtures.get_border_sites()
    for bs in border_sites:
        r = pct.get_indices_tree_with_ghosts(partitioning,(bs,))




@pytest.mark.parametrize(
    "idx,idxp",
    [
        # Site -1 , Site 5 (jumping over border and bulk)
        ((0,0,0,1,0),(0,0,4,0,0)),
    ])
def test_iterate_size_tree_with_holes(idx,idxp):
    partitioners = fixtures.partitioners_list_1D_42()
    partitioning = fixtures.partitioning1D()
    size_tree = pt.get_size_tree(partitioning,
                                 lambda idx : pp.only_NmD1D2_halos(partitioners,
                                                                   idx,1,1) )
    print("size tree:")
    print(tree.tree_str(size_tree))
    assert pt.iterate(size_tree, idx) == idxp
