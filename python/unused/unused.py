#!/usr/bin/env python3
from tree import tree_apply


def repr_from_key(key):
    return ("(" + ", ".join(["{}"] * len(key)) + ")").format(*key)


def _uniquefy_tree(tree):
    def itch(node):
        _, cs = node
        return tuple(cs)

    def pops(node, children):
        m, _ = node
        return (m, tuple(set(children)))

    return tree_apply(tree, itch, pops)
