#!/usr/bin/env python3
from partitioning1D import Partitioning1D
from partitioning import IndexResult


all_leaf1ds = dict()

saved = 0
def Leaf1D_factory(geom_info, dimension):
    key = (geom_info, dimension)
    if key not in all_leaf1ds:
        all_leaf1ds[key] = _Leaf1D(*key)
    else:
        global saved
        saved += 1
    return all_leaf1ds[key]


class _Leaf1D(Partitioning1D):
    def __init__(self, geom_info, dimension):
        self.size, self.parity = geom_info
        self.dimension = dimension

    def _key(self):
        return (self.size, self.parity, self.dimension)

    @property
    def comments(self):
        return f" Leaf1D - Dimension: {self.dimension}, size: {self.size}, parity: {self.parity}"

    def coord_to_idxs(self, relative_x):
        if 0 <= relative_x < self.size:
            return [IndexResult(idx=relative_x, rest=0, cached_flag=False)]
        else:
            return []

    def idx_to_coord(self, idx, offset):
        return idx

    def idx_to_child_kind(self, idx):
        pass

    def max_idx_value(self):
        return self.size
