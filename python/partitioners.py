#!/usr/bin/env python3
from wrappers import objectify
import multidimensional as md


def eo(split_dir, cbflags):
    return objectify(md.eo_fun, split_dir, cbflags)


def q(dimension, nparts):
    return objectify(md.q_fun, dimension, nparts)


def hbb(dimension, halo):
    return objectify(md.hbb_fun, dimension, halo)


def leaf(dimension):
    return objectify(md.leaf_fun, dimension)
