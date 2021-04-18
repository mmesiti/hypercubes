#!/usr/bin/env python3


def get_size_tree(partition_class_tree, partition_exists_pred):
    '''
    Returns a tree of concrete partitions,
    NOT partition classes.
    '''
    leaf = (None, ())

    def size(n):
        print(n)
        (s, idx), cs = n
        return s

    def _get_size_tree(node, top_idx):
        partition_class, children = node
        children_results = ()
        if children != (leaf, ):
            for i in range(partition_class.max_idx_value()):
                new_pc = children[partition_class.idx_to_child_kind(i)]
                new_idx = top_idx + (i, )
                # The predicate needs not the first index
                if partition_exists_pred(new_idx[1:]):
                    r = _get_size_tree(new_pc, new_idx)
                    if size(r) > 0:
                        children_results += (r, )
        else:
            return (partition_class.max_idx_value(), top_idx[-1]), ()

        total_size = sum(size(cr) for cr in children_results)

        return (total_size, top_idx[-1]), children_results

    return _get_size_tree(partition_class_tree, (None, ))


def get_start_tree(size_tree):
    '''
    Depth-first allocation order.
    For other allocation orders,
    swap levels in the tree.
    '''
    def _start_tree(size_tree, start):
        (size, idx), children = size_tree
        starts = tuple(start + sum(s for (s, _1), _2 in children[:i])
                       for i in range(len(children)))
        print("STARTS", starts, "START", start)
        new_children = tuple(
            _start_tree(c, s) for c, s in zip(children, starts))
        return (start, idx), new_children

    return _start_tree(size_tree, 0)


def get_start_idx(size_tree):
    '''
    Return first (depth-first-wise) indices list
    in a tree.
    '''
    (s, idx), children = size_tree

    def get_idx(subtree):
        (_1, idx), _2 = subtree
        return idx

    if children == ():
        return (0, )
    else:
        return (get_idx(children[0]), ) + get_start_idx(children[0])


def iterate(size_tree, idxs):
    '''
    `ptree` is a tree of concrete partitions,
    where each node is in the form
    '''
    def get_idx(subtree):
        (_1, idx), _2 = subtree
        return idx

    def _iterate(size_tree, idxs):
        (s, idx), children = size_tree
        nidx = idxs[0]

        if children == ():  # Leaf case
            new_idx = nidx + 1
            if new_idx == s:
                return None
            else:
                return (nidx + 1, )
        else:  # Node case
            # looking for child with the right idx
            i, c = next(
                (i, c) for i, c in enumerate(children) if get_idx(c) == nidx)

            sub_idxs = _iterate(c, idxs[1:])
            if sub_idxs is None:  # We need to move to the next child
                if i == len(children) - 1:  # We give up
                    return None
                else:
                    sub_idxs = get_start_idx(children[i + 1])
                    return (i + 1, ) + sub_idxs
            else:
                return (i, ) + sub_idxs

    return _iterate(size_tree, idxs)
