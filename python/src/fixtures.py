#!/usr/bin/env python3
import partitioning_tree as pt

from os import getenv


def nexamples(examples):
    if getenv("FAST"):
        return 10
    else:
        return examples


def partitioning4D():
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
        ("EO", tuple(True for _ in sizes), "eo", None),
        ("EO-flattened", EXTRA, "leaf", None),
    )

    return pt.get_partitioning(geom_infos, partitioners_list)


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
    sizes = (42, )

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


def get_bulk_sites():
    tens = "00000001111112222223333334"
    units = "12347892345890345690145690"
    return [int(t) * 10 + int(u) for t, u in zip(tens, units)]


def get_border_sites():
    tens = "0001111222233334"
    units = "0560167127823781"
    return [int(t) * 10 + int(u) for t, u in zip(tens, units)]

