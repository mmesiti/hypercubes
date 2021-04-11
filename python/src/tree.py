#!/usr/bin/env python3
from functools import cache

def tree_apply(
    # the root/node of the tree
    # plus any information
    # that needs to be passed down the tree
    # to the subsequent levels of recursion
    node,
    # a function that creates an iterator on the children
    # (including the information
    # that needs to be passed
    #  to the children)
    iterate_children,
    # a function that aggregates the results
    # it is called last in the recursion,
    # so it basically pops the stack.
    # It is also responsible
    # for terminating the recursion.
    pop_stack,
):

    return pop_stack(
        node,
        tuple(
            tree_apply(n, iterate_children, pop_stack)
            for n in iterate_children(node)
        ),
    )

@cache
def tree_apply_memoized(
    node,
    iterate_children,
    pop_stack,
):
    '''
    Same, but for hashable args
    '''

    return pop_stack(
        node,
        tuple(
            tree_apply_memoized(n, iterate_children, pop_stack)
            for n in iterate_children(node)
        ),
    )


def get_all_paths(node):
    '''
    Get the full paths from the root
    to every leaf of the tree, as a list.
    '''
    def iterate_children(node):
        _, children = node
        return children

    def pop_stack(node, children):
        n, _ = node
        children_lists = children
        if len(children) != 0:  # node case
            # we get a list of lists for each child
            cs = [c for children_list in children_lists for c in children_list]
            return tuple((n,) + child for child in cs)
        else:  # leaf case
            return ((n,),)

    return tree_apply(node, iterate_children, pop_stack)


def get_flat_list_of_subtrees(level, node,
                              get_children = lambda x: x[1],):
    '''
    '''
    def iterate_children(node):
        level, other = node
        children = get_children(other)
        return tuple((level-1, c) for c in children)

    def pop_stack(node, children):
        level, other = node
        children_lists = children
        if len(children) != 0 and level > 0:  # node case
            # we get a list of lists for each child
            return tuple(c for children_list in children_lists for c in children_list)
        else:  # leaf case
            return (other,)

    return tree_apply((level,node), iterate_children, pop_stack)


def get_max_depth(node, get_children = lambda x: x[1]):
    def iterate_children(node):
        return get_children(node)

    def pop_stack(_, children):
        lengths = children
        if lengths:
            return 1 + max(lengths)
        else:  # leaf case
            return 1

    return tree_apply(node, iterate_children, pop_stack)


def uniquefy_tree(tree):
    def itch(node):
        _, cs = node
        return tuple(cs)

    def pops(node, children):
        m, _ = node
        return (m, tuple(set(children)))

    return tree_apply(tree,itch,pops)


def count_elements(tree):
    def itch(node):
        _, cs = node
        return cs

    def pops(node, children):
        m, _ = node
        return 1+sum(children)

    return tree_apply(tree,itch,pops)
