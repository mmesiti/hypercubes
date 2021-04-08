#!/usr/bin/env python3
from tree import tree_apply, get_max_depth, get_all_paths, tree_apply_memoized
from box import Box
from partitioners import partitioners_dict

DEBUG = False
count = 0


def get_partitioning(geom_infos, partitioners):
    """
    Creates a tree of partitionings
    from an initial range specification
    and a list of partitioners
    to be applied sequentially.

    A node is represented
    by the ranges that represent lattice partition.
    """
    def iterate_children(node):
        """
        In this case, node is actually size.
        """
        geom_info, partitioners = node
        (name, dimension_info, partitioner_name,
         partitioner_parameters) = partitioners[0]
        other_partitioners = partitioners[1:]
        partitioner = partitioners_dict[partitioner_name](
            partitioner_parameters)
        partition = partitioner(geom_info, dimension_info)

        children = ([(new_geom_info, other_partitioners)
                     for new_geom_info in partition.sub_geom_info_list()]
                    if other_partitioners else [])
        return children

    def pop_stack(node, children):
        geom_info, partitioners = node
        name, dimension_info, partitioner_name, partitioner_parameters = partitioners[
            0]
        partitioner = partitioners_dict[partitioner_name](
            partitioner_parameters)
        partition = partitioner(geom_info, dimension_info)

        if DEBUG:
            global count
            print(name, geom_info, count)
            count += 1

        return Box(sub_partitionings=children, name=name, partition=partition)

    return tree_apply_memoized(
        node=(geom_infos, partitioners),
        iterate_children=iterate_children,
        pop_stack=pop_stack,
    )


def partitioning_to_str(partitions, prefix, max_level):
    def iterate_children(node):
        n, prefix, max_level = node
        new_prefix = prefix + "   "
        children = n.sub_partitionings
        next_level = []
        if children:
            for child in children[:-1]:
                next_level.append((child, new_prefix + "|", max_level - 1))
            next_level.append((children[-1], new_prefix + " ", max_level - 1))
        return next_level

    def pop_stack(node, children):
        n, prefix, max_level = node
        if max_level == 0:
            return ""
        header_line1 = "".join((prefix, "+", n.name))
        header_line2 = "".join((prefix, " ", n.partition.comments))
        children_lines = children
        return "\n".join([
            header_line1,
            header_line2,
        ] + [line for line in children_lines if len(line) != 0])

    return tree_apply(
        node=(partitions, prefix, max_level),
        iterate_children=iterate_children,
        pop_stack=pop_stack,
    )


def print_partitioning(partitions, prefix, max_level):
    print(partitioning_to_str(partitions, prefix, max_level))


def get_indices_tree(partitioning, coordinates):
    """
    Get index tree.
    A certain location can be reached
    in multiple ways,
    so a bifurcation at that level is necessary.
    """
    def iterate_children(node):
        partitioning, coordinates, name = node
        possible_indices = partitioning.partition.coords_to_idxs(coordinates)
        real_value = next(v for v in possible_indices if not v.cached_flag)
        rest = real_value.rest
        child_type = partitioning.partition.idx_to_child_kind(real_value.idx)
        sub_partitionings = partitioning.sub_partitionings

        return [(sub_partitionings[child_type], rest,
                 partitioning.name)] if sub_partitionings else []

    def pop_stack(node, children):
        partitioning, coordinates, name = node
        possible_indices = partitioning.partition.coords_to_idxs(coordinates)
        real_value = next(v for v in possible_indices if not v.cached_flag)
        return ((real_value.idx), children)

    return tree_apply((partitioning, coordinates, ''), iterate_children,
                      pop_stack)


def get_indices_tree_with_ghosts(partitioning, coordinates):
    """
    Get index tree.
    A certain location can be reached
    in multiple ways,
    so a bifurcation at that level is necessary.
    """
    def iterate_children(node):
        idx, cached_flags, partitioning, coordinates, name = node
        if partitioning:
            possible_indices = partitioning.partition.coords_to_idxs(
                coordinates)
            sub_partitionings = partitioning.sub_partitionings

            return [
                (
                    i.idx,  #
                    i.cached_flag,  #
                    sub_partitionings[partitioning.partition.idx_to_child_kind(
                        i.idx)] if sub_partitionings else None,  #
                    i.rest,  #
                    partitioning.name)  #
                for i in possible_indices
            ]
        else:
            return []

    def pop_stack(node, children):
        idx, cached_flag, partitioning, coordinates, name = node
        return ((idx, cached_flag, name), children)

    return tree_apply(("0", False, partitioning, coordinates, "ROOT"),
                      iterate_children, pop_stack)


def get_relevant_indices_flat(tree_indices):
    max_depth = get_max_depth(tree_indices)
    print(max_depth)
    idxs = get_all_paths(tree_indices)

    return [
        (
            len([_ for _, flag, name in index
                 if flag]),  # count of cache flags
            [(idx, flag, name) for idx, flag, name in index[1:]])
        for index in idxs if len(index) == max_depth
    ]