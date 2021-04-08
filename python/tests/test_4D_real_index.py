#!/usr/bin/env python3
from box import Box
import partitioning_tree as pt
import tree


def test_main():
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

    it = pt.get_indices_tree(partitioning, (20, 13, 23, 4))
    idxs = tree.get_all_paths(it)
    #print(idxs)

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
    # Parity is 0
    # IDX in the eo partition is
    # (2 + 3*1 + 3*4*0 + 3*4*4*3) // 2 = 149 // 2 = 74
    # So 1,1,2,0,1,0,0,0,2,2,2,2,0,74
    #
    expected_idx = [1, 1, 2, 0, 1, 0, 0, 0, 2, 2, 2, 2, 0, 74]

    assert expected_idx == idxs[0]
