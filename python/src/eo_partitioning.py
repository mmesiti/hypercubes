#!/usr/bin/env python3
from partitioning import Partitioning, IndexResult
import eo

all_eos = dict()


class EO(Partitioning):
    def __init__(self, geom_info, cbflags):
        self.sizes = [s for s, p in geom_info]
        self.parities = [p for s, p in geom_info]
        self.cbflags = cbflags

        self.cbsizes = [s for s, f in zip(self.sizes, cbflags) if f]
        self.cumcbsizes = eo.get_cumsizes(self.cbsizes)

        key = (geom_info, cbflags)
        if key not in all_eos:
            all_eos[key] = self

    @property
    def comments(self):
        cbfls = "".join(["T" if f else "F" for f in self.cbflags])
        return f" sizes: {self.sizes}, parities: {self.parities}, nsites: {self.cumcbsizes[-1]} Affected dirs: {cbfls}"

    def sub_geom_info_list(self):
        origin_parity = sum(p for p, f in zip(self.parities, self.cbflags)) % 2

        nsites_with_opposite_parity = self.cumcbsizes[-1] // 2
        nsites_with_origin_parity = self.cumcbsizes[-1] - nsites_with_opposite_parity

        new_sizes = (nsites_with_origin_parity, nsites_with_opposite_parity)
        neven_sites = new_sizes[origin_parity]
        nodd_sites = new_sizes[1 - origin_parity]

        nsites = (neven_sites,) + ((nodd_sites,) if nodd_sites != neven_sites else ())

        old_geom_info_cbremoved = tuple(
            (1, None) if f else s for s, f in zip(self.sizes, self.cbflags)
        )
        return tuple(old_geom_info_cbremoved + ((new_r, None),) for new_r in nsites)

    def coords_to_idxs(self, relative_xs):
        relative_cbxs = tuple(x for x, f in zip(relative_xs, self.cbflags) if f)
        relative_eo_idx, idxh = eo.lex_coord_to_eoidx(relative_cbxs, self.cumcbsizes)
        eo_idx = (relative_eo_idx + sum(self.parities)) % 2

        rests_cbremoved = tuple(
            0 if f else relative_x for relative_x, f in zip(relative_xs, self.cbflags)
        )

        # cached = not all(0 <= x < s for x, s in zip(xs, sizes))
        return [
            IndexResult(
                idx=eo_idx, rest=rests_cbremoved + (idxh,), cached_flag=False  #  #
            )  # cached?
        ]

    def idx_to_coords(self, idx, offsets):
        """
        Returns the relative_x
        given the idx of the partition.
        """
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
        if self.cumcbsizes[-1] % 2 == 0:
            return 0
        else:
            return eo_idx

    def max_idx_value(self):
        return 2
