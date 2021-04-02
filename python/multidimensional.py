#!/usr/bin/env python3
from wrappers import dimensionalize
import monodimensional
from box import Box
import eo


def eo_fun(ranges, split_dir, cbflags):
    '''
    Using the relative rest / relative sub-range choice.
    This also flattens the lattice in the cb-flagged dimensions,
    which are removed and replaced with None.
    '''
    if split_dir is None:
        split_dir = next(d for d, (s, e) in enumerate(ranges)
                         if (e - s) % 2 == 0)

    cbranges = [r for r, f in zip(ranges, cbflags) if f]

    cbsizes = [e - s for s, e in cbranges]
    cumcbsizes = eo.get_cumsizes(cbsizes)

    def get_new_relative_ranges():
        origin_parity = sum(s for s, _ in ranges) % 2

        nsites_with_opposite_parity = cumcbsizes[-1] // 2
        nsites_with_origin_parity = cumcbsizes[-1] - nsites_with_opposite_parity

        new_sizes = [nsites_with_origin_parity, nsites_with_origin_parity]
        neven_sites = new_sizes[origin_parity]
        nodd_sites = new_sizes[1 - origin_parity]

        old_ranges_cbremoved = [[0, 1] if f else r
                                for r, f in zip(ranges, cbflags)]
        return [
            old_ranges_cbremoved + [new_r]
            for new_r in [(0, neven_sites), (0, nodd_sites)]
        ]

    def coord_to_idx(xs):
        cbxs = [x for x, f in zip(xs, cbflags) if f]
        relative_cbxs = [x - s for x, (s, e) in zip(cbxs, cbranges)]

        eo_idx, idxh = eo.lex_coord_to_eoidx(relative_cbxs, cumcbsizes)

        rests_cbremoved = [0 if f else x for x, f in zip(xs, cbflags)]

        # cached = not all(0 <= x < s for x, s in zip(xs, sizes))
        res = Box(
            idx=eo_idx,  #
            rests=rests_cbremoved + [idxh],  #
            cached_flag=False)  # cached?

        return [res]

    def idx_to_coord(idx, offsets):
        dimensionality = len(ranges)
        eo_idx = idx
        idxh = offsets[dimensionality + 1]
        cbcoords = eo.lexeo_idx_to_coord(eo_idx, idxh, cbsizes)
        coordinates = list(offsets[:dimensionality])
        dimensions_to_change = [d for d, f in enumerate(cbflags) if f]
        for cbcoord, dimension in zip(cbcoords, dimensions_to_change):
            start, end = ranges[dimension]
            coordinates[dimension] = cbcoord + start
        return coordinates

    comments = f"Dir: {split_dir}, {ranges}"

    return get_new_relative_ranges(), coord_to_idx, idx_to_coord, comments


def q_fun(ranges, dimension, nparts):
    return dimensionalize(
        monodimensional.q_fun1D,  #
        ranges,  #
        dimension,  #
        nparts)


def hbb_fun(sizes, dimension, halo):
    return dimensionalize(
        monodimensional.hbb_fun1D,  #
        sizes,  #
        dimension,  #
        halo)


def leaf_fun(ranges, dimension):
    return dimensionalize(
        monodimensional.leaf_fun1D,  #
        ranges,  #
        dimension)
