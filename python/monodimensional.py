#!/usr/bin/env python3
from math import ceil
from box import Box


def q_fun1D(size, nparts):
    quotient = ceil(size / nparts)
    rest = size % quotient

    def indexer(x):
        def child_type(idx):
            return 0 if rest == 0 or idx < (nparts - 1) else 1

        idx = x // quotient
        x_rest = x % quotient
        res = Box(idx=idx, rest=x_rest, child_type=child_type(idx))

        real_values = res if 0 <= x < size else None
        return real_values, [
            Box(idx=idx, rest=(x - idx * quotient), child_type=child_type(idx))
            for idx in range(nparts)
        ]

    rest_size = [rest] if rest != 0 else []

    return [quotient] + rest_size, indexer, "q"


def hbb_fun1D(size, halo):
    assert halo != 0

    bulk = size - 2 * halo

    limits = [-halo, 0, halo, size - halo, size, size + halo]
    starts = limits[:-1]
    ends = limits[1:]

    def indexer(x):
        if not -halo <= x < size + halo:
            real_value = None
        else:
            idx, rest = next((i, x - s)  #
                             for i, (s, e) in enumerate(zip(starts, ends))
                             if s <= x < e)

            real_value = Box(
                idx=idx,  #
                rest=rest,  #
                child_type=0 if idx == 2 else 1)
        return real_value, []

    return [bulk, halo], indexer, "hbb"


def leaf_fun1D(size):
    def indexer(x):
        real_value = (Box(idx=x, rest=0, child_type=0)
                      if 0 <= x < size else None)
        return real_value, []

    return [1], indexer, "leaf"
