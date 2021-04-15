#!/usr/bin/env python3
import partition_class_tree as pt
from fixtures import get_bulk_sites, get_border_sites, partitioning1D

import pytest


def print_itwg(itwg):
    def _print_itwg(prefix, data):
        children_results = tuple(_print_itwg(prefix + " ", d) for d in data[1])
        m = prefix + str(data[0])
        return "\n".join((m, ) + children_results)

    print(_print_itwg("", itwg))


@pytest.fixture
def partitioning1D_fixture():
    return partitioning1D()


def site_check(x, expected_copies, partitioning):

    itwg = pt.get_indices_tree_with_ghosts(partitioning, (x, ))
    idxs = pt.get_relevant_indices_flat(itwg)
    assert len(idxs) == expected_copies

    print_itwg(itwg)


@pytest.mark.parametrize("x", get_bulk_sites())
def test_bulk_sites_no_ghosts(x, partitioning1D_fixture):
    site_check(x, 1, partitioning1D_fixture)


@pytest.mark.parametrize("x", get_border_sites())
def test_border_sites_yes_ghosts(x, partitioning1D_fixture):
    site_check(x, 2, partitioning1D_fixture)


if __name__ == "__main__":
    site_check(0, 2, partitioning1D())
    site_check(41, 2, partitioning1D())
