#!/usr/bin/env python3


def get_all_paths(node):
    """
    Get the full paths from the root
    to every leaf of the tree, as a list.
    """
    n, children = node
    children_results = tuple(get_all_paths(n) for n in children)
    if len(children_results) != 0:  # node case
        # we get a list of lists for each child
        cs = [c for children_list in children_results for c in children_list]
        return tuple((n, ) + child for child in cs)
    else:  # leaf case
        return ((n, ), )


def get_flat_list_of_subtrees(level, node):
    (n, children) = node
    children_lists = tuple(
        get_flat_list_of_subtrees(level - 1, c) for c in children)
    if len(children_lists) != 0 and level > 0:  # node case
        # we get a list of lists for each child
        return tuple(c for children_list in children_lists
                     for c in children_list)
    else:  # leaf case
        return ((n, children), )


def get_max_depth(node):
    _, cs = node
    lengths = tuple(get_max_depth(c) for c in cs)
    if lengths:
        return 1 + max(lengths)
    else:  # leaf case
        return 1


def truncate_tree(max_idx_tree, level):
    (m, cs) = max_idx_tree
    if level > 0:
        return m, tuple(truncate_tree(c, level - 1) for c in cs)
    else:
        return m, ()


def get_leaves_list(max_idx_tree):
    n, children = max_idx_tree
    children_results = tuple(get_leaves_list(c) for c in children)
    return tuple(c for cs in children_results
                 for c in cs) if children_results else (n, )


def ziptree(*trees):
    ts = trees
    assert len({len(t) for t in ts}) == 1
    ns = tuple(n for n, _ in ts)
    css = tuple(cs for _, cs in ts)
    children_results = tuple(ziptree(*t) for t in zip(*css))

    return ns, children_results


def nodemap(tree, f):
    n, cs = tree
    children_results = tuple(nodemap(c, f) for c in cs)
    return f(n), children_results


def collapse_level(tree, level_to_collapse, child_to_replace):
    """
    Returns tree with level removed.
    Node at that level are replaced by
    their child selected by child_to_replace.
    Notice that the children to replace
    needs to exist in all subtrees.
    DOES NOT WORK ON LEAVES.
    """
    def _collapse_level(level, node):
        (n, children) = node
        children_results = tuple(
            _collapse_level(level - 1, c) for c in children)
        if level == 0:
            res = children[child_to_replace]
        else:
            res = (n, children_results)
        return res

    return _collapse_level(level_to_collapse, tree)


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
    new_subtrees = tuple(
        collapse_level(tree, level, ic) for ic in range(nchildren))
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
        return tuple(
            (l - 1) if l > lvl_removed else l for l in new_level_ordering[1:])

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
    def _tree_str(prefix, node):
        (m, cs) = node
        children_results = tuple(_tree_str(prefix + "  ", c) for c in cs)
        head = prefix + str(m)
        return "\n".join((head, ) + children_results)

    return _tree_str("", tree)


def first_nodes_list(tree):
    n, cs = tree
    return [n] + (first_nodes_list(cs[0]) if cs else [])


def depth_first_flatten(tree):
    n, cs = tree
    sub_results = (depth_first_flatten(c) for c in cs)
    res = (n, )
    for sub_res in sub_results:
        res = res + sub_res
    return res
