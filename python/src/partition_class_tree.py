#!/usr/bin/env python3
from tree import get_max_depth, get_all_paths, nodemap
from partitioners import partitioners_dict

from functools import cache


def get_partitioning(geom_infos, partitioners):
    """
    Creates a tree of partitionings
    from an initial range specification
    and a list of partitioners
    to be applied sequentially.

    A node is represented
    by the ranges that represent lattice partition.
    """
    @cache
    def _get_partitioning(geom_info, partitioners):
        (name, dimension_info, partitioner_name,
         partitioner_parameters) = partitioners[0]
        other_partitioners = partitioners[1:]
        partitioner = partitioners_dict[partitioner_name](
            partitioner_parameters)
        partition_class = partitioner(geom_info, dimension_info, name)

        children_results = (tuple(
            _get_partitioning(new_geom_info, other_partitioners)
            for new_geom_info in partition_class.sub_geom_info_list())
                            if other_partitioners else ())

        return partition_class, children_results

    return _get_partitioning(geom_infos, partitioners)


def partitioning_to_str(partitions, prefix, max_level):
    (n, children) = partitions
    new_prefix = prefix + "   "
    children_results = []
    if children:
        for child in children[:-1]:
            children_results.append(
                partitioning_to_str(child, new_prefix + "|", max_level - 1))
        children_results.append(
            partitioning_to_str(children[-1], new_prefix + " ", max_level - 1))

    if max_level == 0 or n is None:
        return ""
    header_line1 = "".join((prefix, "+", n.name))
    header_line2 = "".join((prefix, " ", n.comments))
    children_lines = children_results
    return "\n".join([
        header_line1,
        header_line2,
    ] + [line for line in children_lines if len(line) != 0])


def get_indices_tree(partitioning, coordinates):
    """
    Get index tree.
    A certain location can be reached
    in multiple ways,
    so a bifurcation at that level is necessary.
    """
    leaf = (None, ())

    def _get_indices_tree(partitioning, coordinates, name):
        partition_class, children = partitioning
        possible_indices = partition_class.coords_to_idxs(coordinates)
        real_value = next(v for v in possible_indices if not v.cached_flag)

        if children in ((leaf, ), ()):
            children_results = ()
        else:
            rest = real_value.rest
            child_type = partition_class.idx_to_child_kind(real_value.idx)
            r = _get_indices_tree(
                children[child_type],  #
                rest,  #
                partition_class.name)

            children_results = (r, )
        return (real_value.idx, children_results)

    return _get_indices_tree(partitioning, coordinates, "")


def get_indices_tree_with_ghosts(partitioning, coordinates):
    """
    Get index tree.
    A certain location can be reached
    in multiple ways,
    so a bifurcation at that level is necessary.
    """
    def _get_indices_tree_with_ghosts(idx, cached_flags, partitioning,
                                      coordinates, name):
        partition_class, children = partitioning
        if partition_class is None:
            children_results = ()
        else:
            possible_indices = partition_class.coords_to_idxs(coordinates)
            children_results = tuple(
                _get_indices_tree_with_ghosts(
                    i.idx,  #
                    i.cached_flag,  #
                    children[partition_class.idx_to_child_kind(i.idx)]
                    if children else None,  #
                    i.rest,  #
                    partition_class.name,
                )  #
                for i in possible_indices)

        return ((idx, cached_flags, name), children_results)

    return _get_indices_tree_with_ghosts("0", False, partitioning, coordinates,
                                         "ROOT")


def get_relevant_indices_flat(tree_indices):
    max_depth = get_max_depth(tree_indices)
    print(max_depth)
    idxs = get_all_paths(tree_indices)

    return [
        (
            len([_ for _, flag, name in index
                 if flag]),  # count of cache flags
            [(idx, flag, name) for idx, flag, name in index[1:]],
        ) for index in idxs if len(index) == max_depth
    ]


def get_coord_from_idx(partitioning, idx, dimensions):
    def _get_coord_from_idx(partitioning, indices, level):
        (partition_class, children) = partitioning
        if len(indices) > 1:
            idx = indices[0]
            child_kind = partition_class.idx_to_child_kind(idx)
            offsets = _get_coord_from_idx(children[child_kind], indices[1:],
                                          level + 1)
        else:
            offsets = (0, ) * dimensions

        idx = indices[0]

        return partition_class.idx_to_coords(idx, offsets)

    return _get_coord_from_idx(partitioning, idx, 0)


# This requires a tree as produced by
# "tree_partitioning.get_partitioning"
def get_max_idx_tree(partitioning):
    return nodemap(partitioning, lambda n: n.max_idx_value()
                   if n is not None else None)


def validate_idx(partitioning, idx):
    '''
    Make sure that idx is in the right range.
    '''
    leaf = (None, ())

    def _validate(p, idx):
        if p == leaf:
            return True
        partition, children = p
        valid = idx[0] < partition.max_idx_value() and idx[0] >= 0
        return valid and _validate(
            children[partition.idx_to_child_kind(idx[0])], idx[1:])

    return _validate(partitioning, idx)
