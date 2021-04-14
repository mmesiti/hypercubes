#!/usr/bin/env python3
import level_dependencies as ld
import partition_class_tree as pt
import tree
import pytest
import os


def show_partitioning(partitioning, name):
    os.makedirs("examples", exist_ok=True)
    with open(f"examples/{name}-full.txt", 'w') as f:
        f.write(pt.partitioning_to_str(partitioning, '', 20))
    max_idx_tree = pt.get_max_idx_tree(partitioning)
    with open(f"examples/{name}-maxidx.txt", 'w') as f:
        f.write(tree.tree_str(max_idx_tree))
    dm = ld.find_dependency_matrix(max_idx_tree)
    with open(f"examples/{name}-dm.txt", 'w') as f:
        f.write(str(ld.dependency_matrix_to_df(dm)))
    deps = ld.get_levels_dependencies(partitioning)
    with open(f"examples/{name}-deps.txt", 'w') as f:
        f.write(str(deps))


def partitioning1D_level2even():
    sizes = (40, )

    geom_infos = tuple((s, 0) for s in sizes)

    X, EXTRA = list(range(2))

    partitioners_list = (
        ("MPI X", X, "qper", 2),
        ("VECTOR X", X, "qopen", 2),
        ("halos X", X, "hbb", 1),
        ("EO", tuple(True for _ in sizes), "eo", None),
        ("EO-flattened", EXTRA, "leaf", None),
        ("END",None,"end",None)
    )

    return pt.get_partitioning(geom_infos, partitioners_list)


def test_1D_level2even():
    partitioning = partitioning1D_level2even()
    deps = ld.get_levels_dependencies(partitioning)
    show_partitioning(partitioning, "1Dlevel2even")
    assert deps[0] == []
    assert deps[1] == []
    assert deps[2] == []
    assert deps[3] == []
    assert deps[4] == [2, 3]


def partitioning1D_level2odd():
    sizes = (40, )

    geom_infos = tuple((s, 0) for s in sizes)

    X, EXTRA = list(range(2))

    partitioners_list = (
        ("MPI X", X, "qper", 4),
        ("VECTOR X", X, "qopen", 2),
        ("halos X", X, "hbb", 1),
        ("EO", tuple(True for _ in sizes), "eo", None),
        ("EO-flattened", EXTRA, "leaf", None),
        ("END",None,"end",None),
    )

    return pt.get_partitioning(geom_infos, partitioners_list)


def test_1D_level2odd():
    partitioning = partitioning1D_level2odd()
    deps = ld.get_levels_dependencies(partitioning)
    show_partitioning(partitioning, "1Dlevel2odd")
    assert deps[0] == []
    assert deps[1] == []
    assert deps[2] == []
    assert deps[3] == []
    assert deps[4] == [1, 2, 3]


def partitioning1D_localdofs(level1, level2):
    """
    Partitioning with communting levels.
    """
    sizes = (42, 3, 4)

    geom_infos = tuple((s, 0) for s in sizes)

    X, Y, Z, EXTRA = list(range(4))

    dependent_partitioners_list = (
        ("MPI X", X, "qper", 4),
        ("VECTOR X", X, "qopen", 2),
        ("halos X", X, "hbb", 1),
        ("EO", (True, False, False), "eo", None),
        ("EO-flattened", EXTRA, "leaf", None),
        ("END",None,"end",None),
    )

    independent1 = ("LocalDOFs2", Z, "leaf", None)
    independent2 = ("LocalDOFs1", Y, "leaf", None)

    partitioners_list = (dependent_partitioners_list[:level1] +
                         (independent1, ) +
                         dependent_partitioners_list[level1:level2] +
                         (independent2, ) +
                         dependent_partitioners_list[level2:])

    {print(p) for p in partitioners_list}

    return pt.get_partitioning(geom_infos, partitioners_list)


@pytest.mark.parametrize("level1,level2",
                         [(a, b) for a in range(5) for b in range(a + 1, 5)])
def test_1D_localdofs(level1, level2):
    partitioning = partitioning1D_localdofs(level1, level2)
    deps = ld.get_levels_dependencies(partitioning)
    show_partitioning(partitioning, f"localdof-{level1}-{level2}")

    expected_deps = dict(zip(range(7), [[]] * 7))
    dependent_levels = [i for i in range(7) if i not in [level1, level2 + 1]]
    expected_deps[dependent_levels[-1]] = dependent_levels[:-1]

    assert deps == expected_deps


def partitioning4Dnocache():
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
    sizes = (42, 42, 42, 42)

    geom_infos = tuple((s, 0) for s in sizes)

    X, Y, Z, T = list(range(4))
    EXTRA = T + 1

    partitioners_list = (
        ("MPI X", X, "qper", 2),
        ("MPI Y", Y, "qper", 2),
        ("MPI Z", Z, "qper", 2),
        ("MPI T", T, "qper", 2),
        ("VECTOR X", X, "qopen", 2),
        ("VECTOR Y", Y, "qopen", 2),
        ("VECTOR Z", Z, "qopen", 2),
        ("VECTOR T", T, "qopen", 2),
        ("EO", tuple(True for _ in sizes), "eo", None),
        ("EO-flattened", EXTRA, "leaf", None),
        ("END",None,"end",None),
    )

    return pt.get_partitioning(geom_infos, partitioners_list)


def test_4Dnocache():
    partitioning = partitioning4Dnocache()
    deps = ld.get_levels_dependencies(partitioning)
    show_partitioning(partitioning, "4Dnocache")

    assert deps[0] == []
    assert deps[1] == []
    assert deps[2] == []
    assert deps[3] == []
    assert deps[4] == []
    assert deps[5] == []
    assert deps[6] == []
    assert deps[7] == []
    assert deps[8] == []
    assert deps[9] == [0, 1, 2, 3, 4, 5, 6, 7, 8]


def partitioning4Deasy():
    sizes = (40, 40, 40, 40)

    geom_infos = tuple((s, 0) for s in sizes)

    X, Y, Z, T = list(range(4))
    EXTRA = T + 1

    partitioners_list = (
        ("MPI X", X, "qper", 2),
        ("MPI Y", Y, "qper", 2),
        ("MPI Z", Z, "qper", 2),
        ("MPI T", T, "qper", 2),
        ("VECTOR X", X, "qopen", 2),
        ("VECTOR Y", Y, "qopen", 2),
        ("VECTOR Z", Z, "qopen", 2),
        ("VECTOR T", T, "qopen", 2),
        ("halos X", X, "hbb", 1),
        ("halos Y", Y, "hbb", 1),
        ("halos Z", Z, "hbb", 1),
        ("halos T", T, "hbb", 1),
        ("EO", tuple(True for _ in sizes), "eo", None),
        ("EO-flattened", EXTRA, "leaf", None),
        ("END",None,"end",None),
    )

    return pt.get_partitioning(geom_infos, partitioners_list)


def test_4Deasy():
    partitioning = partitioning4Deasy()
    deps = ld.get_levels_dependencies(partitioning)
    show_partitioning(partitioning, "4Deasy")

    assert deps[0] == []
    assert deps[1] == []
    assert deps[2] == []
    assert deps[3] == []
    assert deps[4] == []
    assert deps[5] == []
    assert deps[6] == []
    assert deps[7] == []
    assert deps[8] == []
    assert deps[9] == []
    assert deps[10] == []
    assert deps[11] == []
    assert deps[12] == []
    assert deps[13] == [8, 9, 10, 11, 12]
