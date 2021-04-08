#!/usr/bin/env python3
from partitioning1D import Partitioning1D
from partitioning import IndexResult
from math import ceil


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


bc_funcs = {
    "OPEN": q_idx_range_open,
    "PERIODIC": q_idx_range_periodic,
}


all_q1ds = dict()

saved = 0
def Q1D_factory(geom_info, nparts, dimension, boundary_condition):
    key = (geom_info, nparts, dimension, boundary_condition)
    return _Q1D(*key)
    #if key not in all_q1ds:
    #    all_q1ds[key] = _Q1D(*key)
    #else:
    #    global saved
    #    saved += 1
    #return all_q1ds[key]


class _Q1D(Partitioning1D):
    def __init__(self, geom_info, nparts, dimension, boundary_condition):
        self.size, self.parity = geom_info
        self.dimension = dimension
        self.nparts = nparts
        self.bc = boundary_condition

        assert self.size > nparts

    def _key(self):
        return (self.size, self.parity, self.dimension, self.nparts, self.bc)

    @property
    def comments(self):
        return f" Q1D - Dimension: {self.dimension}, size: {self.size}, parity: {self.parity}, nparts: {self.nparts}"

    @property
    def limits(self):
        quotient = ceil(self.size / self.nparts)
        return [quotient * i for i in range(self.nparts)] + [self.size]

    def coord_to_idxs(self, relative_x):
        min_idx, max_idx = self.q_idx_range_fun(relative_x, self.quotient,
                                                self.nparts)
        real_idx = relative_x // self.quotient
        r = relative_x % self.quotient

        return [
            IndexResult(
                # % is modulo, for boundary conditions
                # NOTE: % is REST instead in C/C++.
                idx=idx % self.nparts,
                rest=r,
                cached_flag=(idx != real_idx))
            for idx in range(min_idx, max_idx)
        ]

    def idx_to_coord(self, idx, offset):
        return self.quotient * idx + offset

    def max_idx_value(self):
        return self.nparts
