#!/usr/bin/env python3
from partitioning import Partitioning, IndexResult
import eo

all_eos = dict()

saved = 0
def EO_factory(geom_info, cbflags):
    key = (geom_info, cbflags)
    #return _EO(*key)
    if key not in all_eos:
        all_eos[key] = _EO(*key)
    else:
        global saved
        saved += 1
    return all_eos[key]


class _EO(Partitioning):
    def __init__(self, geom_info, cbflags):
        self.sizes = [s for s, p in geom_info]
        self.parities = [p for s, p in geom_info]
        self.cbflags = cbflags

        self.cbsizes = [s for s, f in zip(self.sizes, cbflags) if f]
        self.cumcbsizes = eo.get_cumsizes(self.cbsizes)

    @property
    def comments(self):
        cbfls = ''.join(["T" if f else "F" for f in self.cbflags])
        return f" sizes: {self.sizes}, parities: {self.parities}, nsites: {self.cumcbsizes[-1]} Affected dirs: {cbfls}"

    def sub_geom_info_list(self):
        origin_parity = sum(p for p, f in zip(self.parities, self.cbflags)) % 2

        nsites_with_opposite_parity = self.cumcbsizes[-1] // 2
        nsites_with_origin_parity = self.cumcbsizes[
            -1] - nsites_with_opposite_parity

        new_sizes = [nsites_with_origin_parity, nsites_with_opposite_parity]
        neven_sites = new_sizes[origin_parity]
        nodd_sites = new_sizes[1 - origin_parity]

        nsites = [neven_sites
                  ] + ([nodd_sites] if nodd_sites != neven_sites else [])

        old_geom_info_cbremoved = [(1, None) if f else s
                                   for s, f in zip(self.sizes, self.cbflags)]
        return [old_geom_info_cbremoved + [(new_r, None)] for new_r in nsites]

    def coord_to_idxs(self, relative_xs):
        relative_cbxs = [x for x, f in zip(relative_xs, self.cbflags) if f]

        eo_idx, idxh = eo.lex_coord_to_eoidx(relative_cbxs, self.cumcbsizes)

        rests_cbremoved = [
            0 if f else relative_x
            for relative_x, f in zip(relative_xs, self.cbflags)
        ]

        # cached = not all(0 <= x < s for x, s in zip(xs, sizes))
        return [
            IndexResult(
                idx=eo_idx,  #
                rests=rests_cbremoved + [idxh],  #
                cached_flag=False)  # cached?
        ]

    def idx_to_coords(self, idx, offsets):
        '''
        Returns the relative_x
        given the idx of the partition.
        '''
        dimensionality = len(self.sizes)
        eo_idx = idx
        idxh = offsets[dimensionality + 1]
        cbcoords = eo.lexeo_idx_to_coord(eo_idx, idxh, self.cbsizes)
        coordinates = list(offsets[:dimensionality])
        dimensions_to_change = [d for d, f in enumerate(self.cbflags) if f]
        for cbcoord, dimension in zip(cbcoords, dimensions_to_change):
            coordinates[dimension] = cbcoord
        return coordinates

    def idx_to_child_kind(self, idx):
        eo_idx = idx
        #if len(new_geom_info_list) == 1:
        if self.cumcbsizes[-1] % 2 == 0:
            return 0
        else:
            return eo_idx

    def max_idx_value(self):
        return 2
