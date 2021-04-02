#!/usr/bin/env python3
'''
General concepts

The following is a list of functions
that can be used to partition 1D ranges.

Each function takes a range as `geom_info`,
some parameters that define the kind of partitioning
and returns:
- ranges
- indexer function (from a coordinate,
get the index at current level in the hierarchy,
and other infos)
- reverse indexer function (the inverse of the indexer function)
- comments and additional information

Range returned must agree with what the indexer function does.
E.g., range can be (start,end) for each partition
where start and end are absolute coordinates.
But in that case, the "rest" given by the indexer function must be
an absolute coordinate, which means that "rest" must be "x".
If instead the range is (0,subsize), then 0 <= rest < subsize,
which means that rest = x - start.

'''

from math import ceil
from box import Box


def q_idx_range_open(relative_x, quotient, nparts):
    idx_true = relative_x // quotient
    min_idx = max(0, idx_true - 1)
    max_idx = min(nparts, idx_true + 2)

    return min_idx, max_idx


def q_idx_range_periodic(relative_x, quotient, nparts):
    idx_true = relative_x // quotient
    min_idx = idx_true - 1
    max_idx = idx_true + 2

    return min_idx, max_idx


def q_fun1D(geom_info, nparts, q_idx_range_fun=q_idx_range_open):
    '''
    Using the absolute rest / absolute sub-range choice.
    '''
    start, end = geom_info
    size = end - start
    quotient = ceil(size / nparts)

    limits = [start + quotient * i for i in range(nparts)] + [end]
    starts = limits[:-1]
    ends = limits[1:]

    def coord_to_idx(x):
        relative_x = x - start
        min_idx, max_idx = q_idx_range_fun(relative_x, quotient, nparts)

        return [
            Box(
                idx=idx % nparts,  # for boundary conditions
                rest=x,
                cached_flag=(idx != relative_x // quotient))
            for idx in range(min_idx, max_idx)
        ]

    idx_to_coord = None # None because it's not a leaf

    start_ends = list(zip(starts, ends))

    return start_ends, coord_to_idx, idx_to_coord, "q"


def q_fun1D_openbc(geom_info, nparts):
    return q_fun1D(geom_info, nparts, q_idx_range_open)


def q_fun1D_periodicbc(geom_info, nparts):
    return q_fun1D(geom_info, nparts, q_idx_range_periodic)


def hbb_fun1D(geom_info, halo):
    '''
    Using the absolute rest / absolute sub-range choice.
    '''
    assert halo != 0
    start, end = geom_info

    limits = [start - halo, start, start + halo, end - halo, end, end + halo]
    starts = limits[:-1]
    ends = limits[1:]

    def coord_to_idx(x):
        if not start - halo <= x < end + halo:
            return []
        else:
            idx, local_rest = next(
                (i, x - s)  #
                for i, (s, e) in enumerate(zip(starts, ends)) if s <= x < e)

            return [Box(
                idx=idx,  #
                rest=x,  #
                cached_flag=False)]

    idx_to_coord = None  # None because it's not a leaf

    start_ends = list(zip(starts, ends))

    return start_ends, coord_to_idx, idx_to_coord, "hbb"


def leaf_fun1D(geom_info):
    '''
    Here we use the relative rest / relative sub-range  choice.
    '''
    start, end = geom_info

    def coord_to_idx(x):
        if start <= x < end:
            relative_x = x - start
            return [Box(idx=relative_x, rest=0, cached_flag=False)]
        else:
            return []

    def idx_to_coord(idx):
        return start+idx

    return [1], coord_to_idx, idx_to_coord, "leaf"
