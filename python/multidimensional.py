#!/usr/bin/env python3
from wrappers import dimensionalize
import monodimensional
from box import Box


def eo_fun(sizes, split_dir, cbflags):
    if split_dir is None:
        split_dir = next(d for d, s in enumerate(sizes) if s % 2 == 0)
    new_sizes = list(sizes)
    new_sizes[split_dir] //= 2
    assert sizes[split_dir] % 2 == 0, f"{split_dir} = {sizes[split_dir]}"

    def indexer(xs):
        eo = sum(x for x, cbflag in zip(xs, cbflags) if cbflag) % 2
        rests = [x if i != split_dir else x // 2 for i, x in enumerate(xs)]
        #cached = not all(0 <= x < s for x, s in zip(xs, sizes))
        res = Box(
            idx=eo,  #
            rests=rests,  #
            child_type=0,
            cached_flag=False)  # cached?

        return [res]

    comments = f"Dir: {split_dir}, {sizes}"

    return [new_sizes], indexer, comments


def q_fun(sizes, dimension, nparts):
    return dimensionalize(
        monodimensional.q_fun1D,  #
        sizes,  #
        dimension,  #
        nparts)


def hbb_fun(sizes, dimension, halo):
    return dimensionalize(
        monodimensional.hbb_fun1D,  #
        sizes,  #
        dimension,  #
        halo)


def leaf_fun(sizes, dimension):
    return dimensionalize(
        monodimensional.leaf_fun1D,  #
        sizes,  #
        dimension)
