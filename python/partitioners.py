#!/usr/bin/env python3
from eo_partitioning import EO
from hbb1D import HBB1D
from leaf1D import Leaf1D
from q1D import Q1D


def eo(_):
    return lambda geom_info, cbflags: EO(geom_info, cbflags)


def qper(nparts):
    return lambda geom_info, dimension: Q1D(geom_info,  dimension,nparts,
                                            "PERIODIC")


def qopen(nparts):
    return lambda geom_info, dimension: Q1D(geom_info,  dimension,nparts,
                                            "OPEN")


def hbb(halo):
    return lambda geom_info, dimension: HBB1D(geom_info,  dimension, halo)


def leaf(_):
    return lambda geom_info, dimension: Leaf1D(geom_info, dimension)


partitioners_dict = {
    "qper": qper,
    "qopen": qopen,
    "hbb": hbb,
    "eo": eo,
    "leaf": leaf,
}
