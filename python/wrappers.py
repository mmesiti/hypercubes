#!/usr/bin/env python3
from box import Box


def dimensionalize(monodim_partitioner, ranges, dimension, *args, **kwargs):
    '''
    Takes a 1D partitioner and behaves as a multiple dimension one
    '''
    range_ = ranges[dimension]
    (
        new_ranges_list,  #
        indexer,  #
        idx_to_coord,
        comments,  #
    ) = monodim_partitioner(range_, *args, **kwargs)

    new_ranges_list = [[
        new_range if d == dimension else (s, e)
        for d, (s, e) in enumerate(ranges)
    ] for new_range in new_ranges_list]

    def dimensionalized_coord_to_idx(xs):
        possible_indices = indexer(xs[dimension])

        def dimensionalize_rest(value):
            if type(value) == str:
                import pdb
                pdb.set_trace()

            rests = [
                x if i != dimension else value.rest for i, x in enumerate(xs)
            ]
            return Box(idx=value.idx,
                       rests=rests,
                       cached_flag=value.cached_flag)

        return [dimensionalize_rest(i) for i in possible_indices]

    if idx_to_coord:
        def dimensionalized_idx_to_coord(idx, offsets):
            coordinates = list(offsets)
            coordinates[dimension] = idx_to_coord(idx)
            return coordinates
    else:
        def dimensionalized_idx_to_coord(idx, offsets):
            return offsets

    return (
        new_ranges_list,  #
        dimensionalized_coord_to_idx,  #
        dimensionalized_idx_to_coord,  #
        comments + f"  Sizes: {ranges}, Dimension: {dimension}")  #


def objectify(partitioner, *args, **kwargs):
    return lambda ranges: partitioner(ranges, *args, **kwargs)
