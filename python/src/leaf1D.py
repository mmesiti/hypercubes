#!/usr/bin/env python3
from partitioning1D import Partitioning1D
from partitioning import IndexResult
from dimensionalise import dimensionalise


all_leaf1ds = dict()


@dimensionalise
class Leaf1D(Partitioning1D):

    def __init__(self, geom_info, dimension):
        self.size, self.parity = geom_info
        self.dimension = dimension
        if self._key() not in all_leaf1ds:
            all_leaf1ds[self._key()] = self

    def _key(self):
        return (self.size, self.parity, self.dimension)

    @property
    def comments(self):
        return f" Leaf1D - Dimension: {self.dimension}, size: {self.size}, parity: {self.parity}"

    def coord_to_idxs(self, relative_x):
        if 0 <= relative_x < self.size:
            return [IndexResult(idx=relative_x, rest=0, cached_flag=False)]

    def idx_to_coord(self, idx, offset):
        return idx

    def idx_to_child_kind(self, idx):
        pass

    def max_idx_value(self):
        return self.size
