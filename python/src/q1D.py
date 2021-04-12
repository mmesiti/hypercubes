#!/usr/bin/env python3
from partitioning1D import Partitioning1D
from partitioning import IndexResult
from dimensionalise import dimensionalise
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


@dimensionalise
class Q1D(Partitioning1D):
    def __init__(self, geom_info, dimension, name, nparts, boundary_condition):
        self.size, self.parity = geom_info
        self.dimension = dimension
        self.name = name
        self.nparts = nparts
        self.bc = boundary_condition

        self._quotient = ceil(self.size / self.nparts)
        assert self.size > nparts
        assert self._quotient * (self.nparts - 1) < self.size, "Not supported."
        if self._key() not in all_q1ds:
            all_q1ds[self._key()] = self

    def _key(self):
        return (self.size, self.parity, self.dimension, self.name, self.nparts,
                self.bc)

    def __repr__(self):
        key = self._key()
        return ("(" + ", ".join(["{}"] * len(key)) + ")").format(*key)

    @property
    def comments(self):
        fmt = "(size:{}, parity:{}, dimension:{}, name:{}, nparts:{}, bc:{})"
        return f" Q1D " + fmt.format(*self._key())

    @property
    def limits(self):
        return [self._quotient * i for i in range(self.nparts)] + [self.size]

    def coord_to_idxs(self, relative_x):
        min_idx, max_idx = bc_funcs[self.bc](relative_x, self._quotient,
                                             self.nparts)
        real_idx = relative_x // self._quotient
        real_rest = relative_x % self._quotient

        def ghost_limits(idx):
            return idx * self._quotient + (
                self.size - self.nparts * self._quotient) * (idx //
                                                             self.nparts)

        return [
            IndexResult(
                # % is modulo, for boundary conditions
                # NOTE: % is REST instead in C/C++.
                idx=idx % self.nparts,
                rest=relative_x - ghost_limits(idx),
                cached_flag=(idx != real_idx),
            ) for idx in range(min_idx, max_idx)
        ]

    def idx_to_coord(self, idx, offset):
        return self._quotient * idx + offset

    def max_idx_value(self):
        return self.nparts
