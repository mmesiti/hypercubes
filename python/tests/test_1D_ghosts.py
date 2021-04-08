#!/usr/bin/env python3

import partitioning_tree as pt
import tree

import pytest


def print_itwg(itwg):
    def itch(node):
        prefix, data = node
        return [(prefix + " ", d) for d in data[1]]

    def pops(node, cs):
        prefix, data = node
        m = prefix + str(data[0])
        return "\n".join([m] + cs)

    print(tree.tree_apply(("", itwg), itch, pops))


@pytest.fixture
def partitioning1D_fixture():
    return partitioning1D()


def partitioning1D():
    """
    Partitioning expected, for each dimension, until EO level:
    Site index:
    tens:        |0 0000 0 0 000 1 1 1111 1 1 112 2 2 2222 2 2 233 3 3 333 3 3 34 4|
    units:       |0 1234 5 6 789 0 1 2345 6 7 890 1 2 3456 7 8 901 2 3 456 7 8 90 1|
    Sizes, lvl 0: 11               11               11               9
                 |0 0000 0 0 000 0|1 1111 1 1 111 1|2 2222 2 2 222 2|3 333 3 3 33 3|
    Sizes, lvl 1: 6        5       6        5       6        5       5       4
                 |0 0000 0|1 111 1|0 0000 0|1 111 1|0 0000 0|1 111 1|0 000 0|1 11 1|
    border-bulk: |1|2222|3|1|222|3|1|2222|3|1|222|3|1|2222|3|1|222|3|1|222|3|1|22|3|
    In-part idx: |0|0123|0|0|012|0|0|0123|0|0|012|0|0|0123|0|0|012|0|0|012|0|0|01|0|
    In-part size: 1 4    1 1 3   1 1 4    1 1 3   1 1 4    1 1 3   1 1 3   1 1 2  1
    """
    sizes = (42,)

    geom_infos = tuple((s, 0) for s in sizes)

    X, EXTRA = list(range(2))

    partitioners_list = (
        ("MPI X", X, "qper", 4),
        ("VECTOR X", X, "qopen", 2),
        ("halos X", X, "hbb", 1),
        ("EO", tuple(True for _ in sizes), "eo", None),
        ("EO-flattened", EXTRA, "leaf", None),
    )

    return pt.get_partitioning(geom_infos, partitioners_list)


def site_check(x, expected_copies, partitioning):

    itwg = pt.get_indices_tree_with_ghosts(partitioning, (x,))
    idxs = pt.get_relevant_indices_flat(itwg)
    assert len(idxs) == expected_copies

    print_itwg(itwg)


def get_bulk_sites():
    tens = "00000001111112222223333334"
    units = "12347892345890345690145690"
    return [int(t) * 10 + int(u) for t, u in zip(tens, units)]


def get_border_sites():
    tens = "0001111222233334"
    units = "0560167127823781"
    return [int(t) * 10 + int(u) for t, u in zip(tens, units)]


@pytest.mark.parametrize("x", get_bulk_sites())
def test_bulk_sites_no_ghosts(x, partitioning1D_fixture):
    site_check(x, 1, partitioning1D_fixture)


@pytest.mark.parametrize("x", get_border_sites())
def test_border_sites_yes_ghosts(x, partitioning1D_fixture):
    site_check(x, 2, partitioning1D_fixture)


if __name__ == "__main__":
    site_check(0, 2, partitioning1D())
    site_check(41, 2, partitioning1D())
