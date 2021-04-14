#!/usr/bin/env python3
from tree import tree_apply


def get_allocation_sizes(partition_class_tree, partition_exists_predicate):

    leaf = (None, ())

    def itch(node):
        (partition_class, children), top_idx, pred = node
        if children == (leaf, ):
            return ()

        children_to_process = tuple((
            children[partition_class.idx_to_child_kind(i)],  # new partition class
            new_idx,  #
            pred)  #
                                    for i in range(partition_class.max_idx_value())
                                    if pred(new_idx := top_idx + (i, )))

        return children_to_process

    def pops(node, children_results):
        (partition_class, children), top_idx, pred = node
        if not children_results:
            return (partition_class.max_idx_value(),top_idx), ()
        head = sum(cr[0][0] for cr in children_results)

        return (head, top_idx), children_results

    return tree_apply((partition_class_tree,(),partition_exists_predicate), itch, pops)
