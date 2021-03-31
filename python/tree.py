#!/usr/bin/env python3

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
        [
            tree_apply(n, iterate_children, pop_stack)
            for n in iterate_children(node)
        ],
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
            return [[n] + child for child in cs]
        else:  # leaf case
            (n, _) = node
            return [[n]]

    return tree_apply(node, iterate_children, pop_stack)


def get_max_depth(node):
    def iterate_children(node):
        _, children = node
        return children

    def pop_stack(node, children):
        n, _ = node
        lengths = children
        if lengths:
            return 1 + max(lengths)
        else:  # leaf case
            return 1

    return tree_apply(node, iterate_children, pop_stack)
