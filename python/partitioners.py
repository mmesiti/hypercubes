#!/usr/bin/env python3
from eo_partitioning import EO_factory
from hbb1D import HBB1D_factory
from leaf1D import Leaf1D_factory
from q1D import Q1D_factory, q_idx_range_open, q_idx_range_periodic


def eo(_):
    return lambda geom_info, cbflags: EO_factory(geom_info, cbflags)


def qper(nparts):
    return lambda geom_info, dimension: Q1D_factory(geom_info, nparts, dimension,
                                            q_idx_range_periodic)


def qopen(nparts):
    return lambda geom_info, dimension: Q1D_factory(geom_info, nparts, dimension,
                                            q_idx_range_open)


def hbb(halo):
    return lambda geom_info, dimension: HBB1D_factory(geom_info, halo, dimension)


def leaf(_):
    return lambda geom_info, dimension: Leaf1D_factory(geom_info, dimension)


partitioners_dict = {
    "qper" : qper,
    "qopen": qopen,
    "hbb": hbb,
    "eo": eo,
    "leaf": leaf,
}
