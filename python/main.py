#!/usr/bin/env python3
from partitioners import eo, q, hbb, leaf
from box import Box
import partitioning_tree as pt
import tree

sizes = [40, 40, 40, 40]

X, Y, Z, T = list(range(4))

# NO: hbb after EO does not work.
# EO must be at the end
# or possibly at the beginning
# (then leading to unconventional setups)

partitioners_list = [
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

partitioners = pt.get_partitioning(sizes, partitioners_list)
# pt.print_partitioning(partitioners,'',20)
it = pt.get_indices_tree(partitioners, [19, 30, 35, 37])
itwg = pt.get_indices_tree_with_ghosts(partitioners, [19, 30, 35, 37])
idx = tree.get_all_paths(it)
idxs = tree.get_all_paths(itwg)

relevant_idxs = pt.get_relevant_indices_flat(itwg)

for relevant_idx in relevant_idxs:
    for i in relevant_idx:
        print(i)

expected_indices = [
    # mpi
    0,  # 19 // 20,  # ("MPI X", q(X, 2)),
    3,  # 30 // 10,  # ("MPI Y", q(Y, 4)),
    3,  # 35 // 10,  # ("MPI Z", q(Z, 4)),
    3,  # 37 // 10,  # ("MPI T", q(T, 4)),
    # vector
    1,  # 19 // 10,  # ("VECTOR X", q(X, 2)),
    0,  # 0 // 5,  # ("VECTOR Y", q(Y, 2)),
    1,  # 5 // 5,  # ("VECTOR Z", q(Z, 2)),
    1,  # 7 // 5,  # ("VECTOR T", q(T, 2)),
    # eo
    1,  # (9 + 2) % 2,  # ("EO", eo(X, [True for _ in sizes])),
    # hbb
    3,  # ("halos X", hbb(X, 1)), # in last border
    1,  # ("halos Y", hbb(Y, 1)), # in first border
    1,  # ("halos Z", hbb(Z, 1)), # in first border
    2,  # ("halos T", hbb(T, 1)), # in bulk
    # leaf
    0,  # ("leaf X", leaf(X)), # only one site in border
    0,  # ("leaf Y", leaf(Y)), # only one site in border
    0,  # ("leaf Z", leaf(Z)), # only one site in border
    1,  # ("leaf T", leaf(T)), # second site in bulk
]


assert expected_indices == idx[0], f"{expected_indices}, {idx[0]}"
