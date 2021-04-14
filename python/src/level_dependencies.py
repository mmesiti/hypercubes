#!/usr/bin/env python3
import tree
import partition_class_tree as pt


def find_dependency_matrix(max_idx_tree):
    max_depth = tree.get_max_depth(max_idx_tree)-1
    m = dict()

    def check_subtrees_distribution_equal(t):
        '''
        The conditions here may need refinement.
        '''
        subtrees = tree.get_flat_list_of_subtrees(1, t)
        leaves_lists = [
            tree.get_leaves_list(tr) for tr in subtrees
        ]
        # NOTE: if sublist are equal,
        # then this does not mean
        # that the distribution are equal,
        # but this is a necessary condition
        # (except in cases where there is only a single value).
        sublist_equal = all( lista ==
                             listb for lista, listb
                             in zip(leaves_lists[:-1], leaves_lists[1:]))

        # In case sublist have different lentghs
        # but there is only one value
        only_one_value =  len(set.union(*[set(l) for l in leaves_lists])) == 1

        res = sublist_equal or only_one_value
        return res


    for end_depth in range(max_depth):
        m[(end_depth, end_depth)] = True
        truncated = tree.truncate_tree(
            max_idx_tree,
            end_depth)
        for start_depth in range(end_depth):
            subtrees = tree.get_flat_list_of_subtrees(
                start_depth,
                truncated)
            m[(end_depth, start_depth)] = all(
                check_subtrees_distribution_equal(subtree) for subtree in subtrees)
    return m


def dependency_matrix_to_df(dependency_matrix):
    import numpy as np
    import pandas as pd
    max_depth = max(j for i, j in dependency_matrix.keys()) + 1
    m2 = [[
        dependency_matrix.get((end_depth, start_depth), np.nan)
        for end_depth in range(max_depth)
    ] for start_depth in range(max_depth)]

    df = pd.DataFrame(np.array(m2))
    return df


def find_dependencies(dependency_matrix):
    max_depth = max(j for i, j in dependency_matrix.keys()) + 1
    must_come_after = dict()
    for level in range(max_depth):
        ns = [dependency_matrix[(level, i)] for i in range(level + 1)]
        must_come_after[level] = [i for i, a in enumerate(ns) if not a]

    return must_come_after


def get_levels_dependencies(partitioning):
    max_idx_tree = pt.get_max_idx_tree(partitioning)
    dm = find_dependency_matrix(max_idx_tree)
    print("in get levels dependencies")
    print(dependency_matrix_to_df(dm))
    return find_dependencies(dm)
