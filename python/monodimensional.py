#!/usr/bin/env python3
'''
General concepts

The following is a list of functions
that can be used to partition 1D ranges.

Each function takes a size,
some parameters that define the kind of partitioning
and returns:
- sizes
- indexer function (from a relative coordinate,
get the index at current level in the hierarchy,
and other infos)
- reverse indexer function (the inverse of the indexer function)
- comments and additional information

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


def _get_new_geom_info_list(starts, ends, parity):
    parities = [(parity + s) % 2 for s in starts]
    subsizes = [e - s for s, e in zip(starts, ends)]

    all_kinds = set(
        (subsize, parity) for subsize, parity in zip(subsizes, parities))

    return list(all_kinds)


def _idx_to_subsize(idx, starts, ends):
    return ends[idx] - starts[idx]


def _idx_to_start_parity(idx, starts, parity):
    return (starts[idx] + parity) % 2


def _get_idx_to_child_kind(starts, ends, new_geom_info_list, parity):
    def idx_to_child_kind(idx):
        for i, (subsize, parity) in enumerate(new_geom_info_list):
            subsize_ = _idx_to_subsize(idx, starts, ends)
            parity_ = _idx_to_start_parity(idx, starts, parity)
            if (subsize_, parity_) == (subsize, parity):
                return i

    return idx_to_child_kind


def q_fun1D(geom_info, nparts, q_idx_range_fun=q_idx_range_open):
    size, parity = geom_info
    assert size > nparts
    quotient = ceil(size / nparts)
    limits = [quotient * i for i in range(nparts)] + [size]
    starts = limits[:-1]
    ends = limits[1:]

    new_geom_info_list = _get_new_geom_info_list(starts, ends, parity)

    def coord_to_idxs(relative_x):
        min_idx, max_idx = q_idx_range_fun(relative_x, quotient, nparts)
        real_idx = relative_x // quotient
        r = relative_x % quotient

        return [
            Box(
                # % is modulo, for boundary conditions
                # NOTE: % is REST instead in C/C++.
                idx=idx % nparts,
                rest=r,
                cached_flag=(idx != real_idx))
            for idx in range(min_idx, max_idx)
        ]

    def idx_to_coord(idx, offset):
        return quotient * idx + offset

    idx_to_child_kind = _get_idx_to_child_kind(starts, ends,
                                               new_geom_info_list, parity)

    return Box(new_geom_info_list=new_geom_info_list,
               coord_to_idxs=coord_to_idxs,
               idx_to_coord=idx_to_coord,
               idx_to_child_kind=idx_to_child_kind,
               comments="q")


def q_fun1D_openbc(geom_info, nparts):
    return q_fun1D(geom_info, nparts, q_idx_range_open)


def q_fun1D_periodicbc(geom_info, nparts):
    return q_fun1D(geom_info, nparts, q_idx_range_periodic)


def hbb_fun1D(geom_info, halo):
    assert halo != 0
    size, parity = geom_info
    assert size > 2*halo

    limits = [-halo, 0, halo, size - halo, size, size + halo]
    starts = limits[:-1]
    ends = limits[1:]

    new_geom_info_list = _get_new_geom_info_list(starts, ends, parity)

    def coord_to_idxs(relative_x):
        if not limits[0] <= relative_x < limits[-1]:
            return []
        else:
            idx, local_rest = next(
                (i, relative_x - s)  #
                for i, (s, e) in enumerate(zip(starts, ends))
                if s <= relative_x < e)

            return [Box(
                idx=idx,  #
                rest=local_rest,  #
                cached_flag=False)]

    def idx_to_coord(idx, offset):
        return starts[idx] + offset

    idx_to_child_kind = _get_idx_to_child_kind(starts, ends,
                                               new_geom_info_list, parity)

    return Box(new_geom_info_list=new_geom_info_list,
               coord_to_idxs=coord_to_idxs,
               idx_to_coord=idx_to_coord,
               idx_to_child_kind=idx_to_child_kind,
               comments="hbb")


def leaf_fun1D(geom_info):
    size, parity = geom_info

    def coord_to_idxs(relative_x):
        if 0 <= relative_x < size:
            return [Box(idx=relative_x, rest=0, cached_flag=False)]
        else:
            return []

    def idx_to_coord(idx, offset):
        return idx

    idx_to_child_kind = None

    return Box(new_geom_info_list=[None],
               coord_to_idxs=coord_to_idxs,
               idx_to_coord=idx_to_coord,
               idx_to_child_kind=idx_to_child_kind,
               comments=f"leaf")
