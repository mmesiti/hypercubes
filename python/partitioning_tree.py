#!/usr/bin/env python3
from tree import tree_apply, get_max_depth, get_all_paths, tree_apply_memoized
from box import Box
from partitioners import partitioners_dict

DEBUG = False
count = 0


def unpack_geom_infos(geom_infos, dimension_info):
    if type(dimension_info) == tuple:
        return tuple([
            geom_info for geom_info, f in zip(geom_infos, dimension_info) if f
        ])
    elif type(dimension_info) == int:
        return geom_infos[dimension_info]
    else:
        raise ValueError("dimension info should be a tuple.")


def repack_geom_infos(old_geom_infos, new_geom_infos, dimension_info):
    if type(dimension_info) == tuple:
        indices = [i for i, f in enumerate(dimension_info) if f]
        res = list(old_geom_infos)
        for i, new_geom_info in zip(indices, new_geom_infos):
            res[i] = new_geom_info
        res += new_geom_infos[len(dimension_info):]
    elif type(dimension_info) == int:
        res = list(old_geom_infos)
        res[dimension_info] = new_geom_infos
    else:
        raise ValueError("dimension info should be a tuple.")

    return tuple(res)


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
        name, dimension_info, partitioner_name, partitioner_parameters = partitioners[
            0]
        other_partitioners = partitioners[1:]
        unpacked_geom_info = unpack_geom_infos(geom_info, dimension_info)
        partitioner = partitioners_dict[partitioner_name](partitioner_parameters)
        partition = partitioner(unpacked_geom_info, dimension_info)

        children = ([
            (repack_geom_infos(geom_info, new_geom_info, dimension_info),
             other_partitioners)
            for new_geom_info in partition.sub_geom_info_list()
        ] if other_partitioners else [])
        return children

    def pop_stack(node, children):
        geom_info, partitioners = node
        name, dimension_info, partitioner_name, partitioner_parameters = partitioners[
            0]
        unpacked_geom_info = unpack_geom_infos(geom_info, dimension_info)
        partitioner = partitioners_dict[partitioner_name](partitioner_parameters)
        partition = partitioner(unpacked_geom_info, dimension_info)

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
        possible_indices = partitioning.indexer(coordinates)
        real_value = next(v for v in possible_indices if not v.cached_flag)
        (rests, child_type) = (
            real_value.rests,
            real_value.child_type,
        )
        sub_partitionings = partitioning.sub_partitionings

        return [(sub_partitionings[child_type], rests,
                 partitioning.name)] if sub_partitionings else []

    def pop_stack(node, children):
        partitioning, coordinates, name = node
        coordinates
        possible_indices = partitioning.partition.coord_to_idxs(unpacked_coordinates)
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
            possible_indices = partitioning.indexer(coordinates)
            sub_partitionings = partitioning.sub_partitionings

            return [
                (
                    i.idx,  #
                    i.cached_flag,  #
                    sub_partitionings[i.child_type]
                    if sub_partitionings else None,  #
                    i.rests,  #
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
    idxs = get_all_paths(tree_indices)

    return [
        (
            len([_ for _, flag, name in index
                 if flag]),  # count of cache flags
            [(idx, flag, name) for idx, flag, name in index[1:]])
        for index in idxs if len(index) == max_depth
    ]
