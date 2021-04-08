#!/usr/bin/env python3
from partitioning1D import Partitioning1D
from partitioning import IndexResult
from dimensionalise import dimensionalise

all_hbb1ds = dict()


@dimensionalise
class HBB1D(Partitioning1D):
    def __init__(self, geom_info, dimension, halo):
        self.size, self.parity = geom_info
        self.dimension = dimension
        self.halo = halo

        assert halo != 0
        assert self.size > 2 * halo
        key = (geom_info, halo, dimension)
        if key not in all_hbb1ds:
            all_hbb1ds[key] = self

    def _key(self):
        return (self.size, self.parity, self.dimension, self.halo)

    @property
    def limits(self):
        return [
            -self.halo,
            0,
            self.halo,
            self.size - self.halo,
            self.size,
            self.size + self.halo,
        ]

    @property
    def comments(self):
        return f" HBB1D - Dimension: {self.dimension}, size: {self.size}, parity: {self.parity}, halo: {self.halo}"

    def coord_to_idxs(self, relative_x):
        if not self.limits[0] <= relative_x < self.limits[-1]:
            return []
        else:
            idx, local_rest = next(
                (i, relative_x - s)  #
                for i, (s, e) in enumerate(zip(self.starts, self.ends))
                if s <= relative_x < e
            )
            return [IndexResult(idx=idx, rest=local_rest, cached_flag=False)]  #  #

    def idx_to_coord(self, idx, offset):
        return self.starts[idx] + offset

    def max_idx_value(self):
        return 5
