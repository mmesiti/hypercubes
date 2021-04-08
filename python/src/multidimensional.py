#!/usr/bin/env python3
from wrappers import dimensionalize
import monodimensional
from box import Box
import eo


def eo_fun(geom_info, cbflags):
    '''
    Using the relative rest / relative sub-range choice.
    This also flattens the lattice in the cb-flagged dimensions,
    which are removed and replaced with None.
    '''
    sizes = [s for s, p in geom_info]
    parities = [p for s, p in geom_info]

    cbsizes = [s for s, f in zip(sizes, cbflags) if f]
    cumcbsizes = eo.get_cumsizes(cbsizes)

    def get_new_geom_info_list():
        origin_parity = sum(p for p, f in zip(parities, cbflags)) % 2

        nsites_with_opposite_parity = cumcbsizes[-1] // 2
        nsites_with_origin_parity = cumcbsizes[-1] - nsites_with_opposite_parity

        new_sizes = [nsites_with_origin_parity, nsites_with_opposite_parity]
        neven_sites = new_sizes[origin_parity]
        nodd_sites = new_sizes[1 - origin_parity]

        nsites = [neven_sites] + ([nodd_sites] if nodd_sites != neven_sites else [])

        old_geom_info_cbremoved = [(1, None) if f else s
                                   for s, f in zip(sizes, cbflags)]
        return [old_geom_info_cbremoved + [(new_r, None)] for new_r in nsites]

    new_geom_info_list = get_new_geom_info_list()

    def coords_to_idx(relative_xs):
        relative_cbxs = [x for x, f in zip(relative_xs, cbflags) if f]

        eo_idx, idxh = eo.lex_coord_to_eoidx(relative_cbxs, cumcbsizes)

        rests_cbremoved = [
            0 if f else relative_x
            for relative_x, f in zip(relative_xs, cbflags)
        ]

        # cached = not all(0 <= x < s for x, s in zip(xs, sizes))
        res = Box(
            idx=eo_idx,  #
            rests=rests_cbremoved + [idxh],  #
            cached_flag=False)  # cached?

        return [res]

    def idx_to_coord(idx, offsets):
        dimensionality = len(sizes)
        eo_idx = idx
        idxh = offsets[dimensionality + 1]
        cbcoords = eo.lexeo_idx_to_coord(eo_idx, idxh, cbsizes)
        coordinates = list(offsets[:dimensionality])
        dimensions_to_change = [d for d, f in enumerate(cbflags) if f]
        for cbcoord, dimension in zip(cbcoords, dimensions_to_change):
            coordinates[dimension] = cbcoord
        return coordinates

    def idx_to_child_kind(idx):
        eo_idx = idx
        if len(new_geom_info_list) == 1:
            return 0
        else:
            return eo_idx

    cbfls = ''.join(["T" if f else "F" for f in cbflags])
    comments = f" Geom info: {geom_info}, nsites: {cumcbsizes[-1]} Affected dirs: {cbfls}"

    return Box(new_geom_info_list=new_geom_info_list,
               coords_to_idxs=coords_to_idx,
               idx_to_coords=idx_to_coord,
               idx_to_child_kind=idx_to_child_kind,
               comments=comments)


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
