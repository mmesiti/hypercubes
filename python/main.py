#!/usr/bin/env python3
from partitioners import eo, q, hbb, leaf
from box import Box

sizes = [40, 40, 40, 40]

X, Y, Z, T = list(range(4))

partitioners = [
    ("MPI X", q(X, 2)),
    ("MPI Y", q(Y, 4)),
    ("MPI Z", q(Z, 4)),
    ("MPI T", q(T, 4)),
    ("VECTOR X", q(X, 2)),
    ("VECTOR Y", q(Y, 2)),
    ("VECTOR Z", q(Z, 2)),
    ("VECTOR T", q(T, 2)),
    ("EO", eo(X, [True for _ in sizes])),
    ("halos X", hbb(X, 1)),
    ("halos Y", hbb(Y, 1)),
    ("halos Z", hbb(Z, 1)),
    ("halos T", hbb(T, 1)),
    ("leaf X", leaf(X)),
    ("leaf Y", leaf(Y)),
    ("leaf Z", leaf(Z)),
    ("leaf T", leaf(T)),
]


def get_partitioning(sizes, partitioners):

    name, partitioner = partitioners[0]
    other_partitioners = partitioners[1:]
    new_sizes_list, indexer, comments = partitioner(sizes)

    child_partitioning = [
        get_partitioning(new_sizes, other_partitioners)
        for new_sizes in new_sizes_list if other_partitioners
    ]

    return Box(child_partitioning=child_partitioning,
               indexer=indexer,
               name=name,
               comments=comments)


def print_partitioning(partitions, prefix, max_level):

    children = partitions.child_partitioning
    if max_level == 0:
        return
    print(prefix, '+', partitions.name)
    print(prefix, ' ', partitions.comments)

    new_prefix = prefix + "   "
    if children:
        for child in children[:-1]:
            print_partitioning(child, new_prefix + '|',max_level-1)
        print_partitioning(children[-1], new_prefix + ' ',max_level-1)


def get_indices(partitioning, coordinates):

    real_value , ghosts = partitioning.indexer(coordinates)
    (idx, rests, child_type) = real_value.idx, real_value.rests, real_value.child_type
    print(partitioning.name)
    sub_partitioning = partitioning.child_partitioning
    return [idx] + get_indices(sub_partitioning[child_type],
                               rests) if sub_partitioning else []
