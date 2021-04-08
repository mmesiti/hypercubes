#!/usr/bin/env python3
from box import Box
import partitioning_tree as pt
import tree

sizes = [42, 42, 42, 42]

geom_infos = tuple([(s, 0) for s in sizes])

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

partitioning = pt.get_partitioning(geom_infos, partitioners_list)

tree_representation = pt.partitioning_to_str(partitioning, '', 15)
with open("tree_repr.txt", 'w') as f:
    f.write(tree_representation)

from q1D import all_q1ds
from leaf1D import all_leaf1ds
from hbb1D import all_hbb1ds
from eo_partitioning import all_eos

with open("all_partitions.txt", 'w') as f:
    for container in [all_hbb1ds, all_leaf1ds, all_q1ds, all_eos]:
        f.write(f"n. elements: {len(container)}\n")
        for k, v in container.items():
            f.write(v.comments + "\n")

#print(pt.get_indices_tree(partitioning, (20,13,23,4)))
