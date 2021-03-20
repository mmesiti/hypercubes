#!/usr/bin/env python3
from box import Box


def dimensionalize(monodim_partitioner, sizes, dimension, *args, **kwargs):
    '''
    Takes a 1D partitioner and behaves as a multiple dimension one
    '''
    size = sizes[dimension]
    (
        new_size_list,  #
        indexer,  #
        comments,  #
    ) = monodim_partitioner(size, *args, **kwargs)

    new_sizes_list = [[
        new_size if d == dimension else s for d, s in enumerate(sizes)
    ] for new_size in new_size_list]

    def dimensionalized_indexer(xs):
        real_value, ghosts = indexer(xs[dimension])

        def dimensionalize_rest(value):
            rests = [
                x if i != dimension else value.rest for i, x in enumerate(xs)
            ]
            return Box(idx=value.idx, rests=rests, child_type=value.child_type)

        return dimensionalize_rest(real_value), [
            dimensionalize_rest(g) for g in ghosts
        ]

    return (
        new_sizes_list,  #
        dimensionalized_indexer,  #
        comments + f"  Sizes: {sizes}, Dimension: {dimension}")  #


def objectify(partitioner, *args, **kwargs):
    return lambda sizes: partitioner(sizes, *args, **kwargs)
