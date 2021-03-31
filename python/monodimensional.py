#!/usr/bin/env python3
from math import ceil
from box import Box


def q_idx_range_open(x, quotient, nparts):
    idx_true = x // quotient
    min_idx = max(0, idx_true - 1)
    max_idx = min(nparts, idx_true + 2)

    return min_idx, max_idx


def q_idx_range_periodic(x, quotient, nparts):
    idx_true = x // quotient
    min_idx = idx_true - 1
    max_idx = idx_true + 2

    return min_idx, max_idx


def q_fun1D(size, nparts, q_idx_range_fun=q_idx_range_open):
    quotient = ceil(size / nparts)
    rest = size % quotient

    def indexer(x):
        def child_type(idx):
            return 0 if rest == 0 or idx < (nparts - 1) else 1

        min_idx, max_idx = q_idx_range_fun(x, quotient, nparts)

        return [
            Box(idx=idx % nparts,
                rest=(x - idx * quotient),
                child_type=child_type(idx),
                cached_flag=(idx != x // quotient))
            for idx in range(min_idx, max_idx)
        ]

    rest_size = [rest] if rest != 0 else []

    return [quotient] + rest_size, indexer, "q"


def q_fun1D_openbc(size, nparts):
    return q_fun1D(size, nparts, q_idx_range_open)


def q_fun1D_periodicnc(size, nparts):
    return q_fun1D(size, nparts, q_idx_range_periodic)


def hbb_fun1D(size, halo):
    assert halo != 0

    bulk = size - 2 * halo

    limits = [-halo, 0, halo, size - halo, size, size + halo]
    starts = limits[:-1]
    ends = limits[1:]

    def indexer(x):
        if not -halo <= x < size + halo:
            return []
        else:
            idx, rest = next((i, x - s)  #
                             for i, (s, e) in enumerate(zip(starts, ends))
                             if s <= x < e)

            return [
                Box(
                    idx=idx,  #
                    rest=rest,  #
                    child_type=(0 if idx == 2 else 1),
                    cached_flag=False)
            ]

    return [bulk, halo], indexer, "hbb"


def leaf_fun1D(size):
    def indexer(x):
        if 0 <= x < size:
            return [Box(idx=x, rest=0, child_type=0, cached_flag=False)]
        else:
            return []

    return [1], indexer, "leaf"


