#!/usr/bin/env python3
from tree import tree_apply


# This requires a tree as produced by
# "tree_partitioning.get_partitioning"
def get_max_idx_tree(partitioning):
    def itch(node):
        n, cs = node
        return cs

    def pops(node, children):
        n, cs = node
        return (n.max_idx_value(), children)

    return tree_apply(partitioning, itch, pops)


# This requires a tree as produced by
# get_max_idx_tree
def max_idx_tree_str(max_idx_tree):
    def itch(node):
        prefix, (_, cs) = node
        return tuple((prefix + '  ', c) for c in cs)

    def pops(node, cs):
        prefix, (m, _) = node
        head = prefix + str(m)
        return '\n'.join((head, ) + cs)

    return tree_apply(('', max_idx_tree), itch, pops)
