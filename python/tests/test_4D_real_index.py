#!/usr/bin/env python3
import partition_class_tree as pt
import tree
import pytest
from hypothesis import given, settings, HealthCheck
from hypothesis import strategies as st
from fixtures import (
    get_bulk_sites,
    get_border_sites,
    partitioning4D,
    nexamples,
    partitioning1D,
)


@pytest.fixture
def bulk_indices():
    return get_bulk_sites()


@pytest.fixture
def border_indices():
    return get_border_sites()


@pytest.fixture(scope="session")
def partitioning4Dfixture():
    return partitioning4D()


@pytest.fixture(scope="session")
def partitioning1Dfixture():
    return partitioning1D()


def test_real_only_1D(partitioning1Dfixture):
    it = pt.get_indices_tree(partitioning1Dfixture, (20,))
    idxs = tree.get_all_paths(it)
    exp_idx = (1, 1, 2, 0, 1)
    assert exp_idx == idxs[0]


def test_real_only(partitioning4Dfixture):

    tree_representation = pt.partitioning_to_str(partitioning4Dfixture, "", 15)
    with open("tree_repr.txt", "w") as f:
        f.write(tree_representation)

    from q1D import all_q1ds
    from leaf1D import all_leaf1ds
    from hbb1D import all_hbb1ds
    from eo_partitioning import all_eos

    with open("all_partitions.txt", "w") as f:
        for container in [all_hbb1ds, all_leaf1ds, all_q1ds, all_eos]:
            f.write(f"n. elements: {len(container)}\n")
            for k, v in container.items():
                f.write(v.comments + "\n")

    it = pt.get_indices_tree(partitioning4Dfixture, (20, 13, 23, 4))
    idxs = tree.get_all_paths(it)

    # Partitioning expected, for each dimension, until EO level:
    # Site index:
    # tens:        |0 0000 0 0 000 1 1 1111 1 1 112 2 2 2222 2 2 233 3 3 333 3 3 34 4|
    # units:       |0 1234 5 6 789 0 1 2345 6 7 890 1 2 3456 7 8 901 2 3 456 7 8 90 1|
    # Sizes, lvl 0: 11               11               11               9
    #              |0 0000 0 0 000 0|1 1111 1 1 111 1|2 2222 2 2 222 2|3 333 3 3 33 3|
    # Sizes, lvl 1: 6        5       6        5       6        5       5       4
    #              |0 0000 0|1 111 1|0 0000 0|1 111 1|0 0000 0|1 111 1|0 000 0|1 11 1|
    # border-bulk: |1|2222|3|1|222|3|1|2222|3|1|222|3|1|2222|3|1|222|3|1|222|3|1|22|3|
    # In-part idx: |0|0123|0|0|012|0|0|0123|0|0|012|0|0|0123|0|0|012|0|0|012|0|0|01|0|
    # In-part size: 1 4    1 1 3   1 1 4    1 1 3   1 1 4    1 1 3   1 1 3   1 1 2  1
    #                    X              X         X     X
    #                   (4)            (13)      (20)  (23)
    # for (20,13,23,4):
    # X 20 -> 1,1,2, in-part idx: 2, in-part size: 3
    # Y 13 -> 1,0,2, in-part idx: 1, in-part size: 4
    # Z 23 -> 2,0,2, in-part idx: 0, in-part size: 4
    # T 4  -> 0,0,2, in-part idx: 3, in-part size: 4
    # Parity is: 0
    # IDX in the eo partition is
    # (2 + 3*1 + 3*4*0 + 3*4*4*3) // 2 = 149 // 2 = 74
    # So, the expected index is
    expected_idx = (1, 1, 2, 0, 1, 0, 0, 0, 2, 2, 2, 2, 0, 74)
    #              |----------------------------------| |  |
    #                         transpose of              |  |
    #                       XYZT idxs/sub-idxs          |  |
    #                                                   |  |
    #                                              parity  |
    #                                               in-eo idx

    assert expected_idx == idxs[0]


@settings(
    max_examples=nexamples(1000),
    suppress_health_check=[HealthCheck.function_scoped_fixture],
)
@given(xs=st.lists(st.integers(min_value=0, max_value=41), min_size=4, max_size=4))
def test_ghosts(xs, partitioning4Dfixture, border_indices, bulk_indices):
    nborders = len([x for x in xs if x in border_indices])
    nbulk = len([x for x in xs if x in bulk_indices])
    assert nbulk + nborders == 4

    itwg = pt.get_indices_tree_with_ghosts(partitioning4Dfixture, xs)
    idxs = pt.get_relevant_indices_flat(itwg)
    assert len(idxs) == 2 ** nborders


if __name__ == "__main__":
    test_real_only(partitioning4D())
