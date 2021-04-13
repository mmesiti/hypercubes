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
            tree_apply(n, iterate_children, pop_stack) for n in iterate_children(node)
        ),
    )


@cache
def tree_apply_memoized(
    node,
    iterate_children,
    pop_stack,
):
    """
    Same, but for hashable args
    """

    return pop_stack(
        node,
        tuple(
            tree_apply_memoized(n, iterate_children, pop_stack)
            for n in iterate_children(node)
        ),
    )


def get_all_paths(node):
    """
    Get the full paths from the root
    to every leaf of the tree, as a list.
    """

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


def get_flat_list_of_subtrees(level, node):
    def iterate_children(node):
        level, (n, children) = node
        return tuple((level - 1, c) for c in children)

    def pop_stack(node, children):
        level, (n, cs) = node
        children_lists = children
        if len(children) != 0 and level > 0:  # node case
            # we get a list of lists for each child
            return tuple(c for children_list in children_lists for c in children_list)
        else:  # leaf case
            return ((n, cs),)

    return tree_apply((level, node), iterate_children, pop_stack)


def get_max_depth(node):
    def iterate_children(node):
        _, children = node
        return children

    def pop_stack(_, children):
        lengths = children
        if lengths:
            return 1 + max(lengths)
        else:  # leaf case
            return 1

    return tree_apply(node, iterate_children, pop_stack)


def _uniquefy_tree(tree):
    def itch(node):
        _, cs = node
        return tuple(cs)

    def pops(node, children):
        m, _ = node
        return (m, tuple(set(children)))

    return tree_apply(tree, itch, pops)


def truncate_tree(max_idx_tree, level):
    def itch(node):
        level, (_, cs) = node
        if level > 0:
            return tuple((level - 1, c) for c in cs)
        else:
            return ()

    def pops(node, cs):
        level, (m, _) = node
        return (m, cs)

    return tree_apply((level, max_idx_tree), itch, pops)


def get_leaves_list(max_idx_tree):
    def itch(node):
        _, cs = node
        return cs

    def pops(node, css):
        n, _ = node
        return tuple(c for cs in css for c in cs) if css else (n,)

    return tree_apply(max_idx_tree, itch, pops)


def ziptree(*trees):
    def itch(node):
        print("itch", node)
        ts = node
        assert len({len(t) for t in ts}) == 1
        css = tuple(cs for _, cs in ts)
        return tuple(zip(*css))

    def pops(node, children_results):
        ts = node
        ns = tuple(n for n, _ in ts)
        return ns, children_results

    return tree_apply(trees, itch, pops)


def nodemap(tree, f):
    def itch(node):
        _, cs = node
        return cs

    def pops(node, children_results):
        n, _ = node
        return f(n), children_results

    return tree_apply(tree, itch, pops)


def collapse_level(tree, level_to_collapse, child_to_replace):
    """
    Returns tree with level removed.
    Node at that level are replaced by
    their child selected by child_to_replace.
    Notice that the children to replace
    needs to exist in all subtrees.
    DOES NOT WORK ON LEAVES.
    """

    def itch(node):
        level, (_, cs) = node
        return tuple(((level - 1), c) for c in cs)

    def pops(node, children_results):
        level, (n, children) = node
        if level == 0:
            res = children[child_to_replace]
        else:
            res = (n, children_results)
        return res

    return tree_apply((level_to_collapse, tree), itch, pops)


def bring_level_on_top(tree, level):
    """
    DOES NOT WORK ON LEAVES.
    """

    def find_nchildren_level(tree, level):
        tt = truncate_tree(tree, level + 1)
        subtrees = get_flat_list_of_subtrees(level, tt)
        nschildren = set(len(get_leaves_list(st)) for st in subtrees)
        assert len(nschildren) == 1
        return nschildren.pop()

    def find_node_representative(tree, level):
        tt = truncate_tree(tree, level)
        leaves_set = set(get_leaves_list(tt))
        assert len(leaves_set) == 1
        return leaves_set.pop()

    nchildren = find_nchildren_level(tree, level)
    node_representative = find_node_representative(tree, level)
    new_subtrees = tuple(collapse_level(tree, level, ic) for ic in range(nchildren))
    return node_representative, new_subtrees


def swap_levels(tree, new_level_ordering):
    """
    Al levels must be nodes, not leaves.

    For convenience, this does not use
    the iterate_children/pop stack pattern,
    as there is a lot of shared code
    between the two functions.
    """

    def get_new_sub_level_ordering(new_level_ordering):
        lvl_removed = new_level_ordering[0]
        return tuple((l - 1) if l > lvl_removed else l for l in new_level_ordering[1:])

    if len(new_level_ordering) == 0:
        return tree
    next_level = new_level_ordering[0]
    new_tree = bring_level_on_top(tree, next_level)
    sub_level_ordering = get_new_sub_level_ordering(new_level_ordering)
    new_node, children = new_tree
    new_children = tuple(swap_levels(c, sub_level_ordering) for c in children)
    return new_node, new_children


# This requires a tree as produced by
# get_max_idx_tree
def tree_str(tree):
    def itch(node):
        prefix, (_, cs) = node
        return tuple((prefix + "  ", c) for c in cs)

    def pops(node, children_results):
        prefix, (m, _) = node
        head = prefix + str(m)
        return "\n".join((head,) + children_results)

    return tree_apply(("", tree), itch, pops)
