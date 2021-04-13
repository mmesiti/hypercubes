#!/usr/bin/env python3
from eo_partitioning import EO
from hbb1D import HBB1D
from leaf1D import Leaf1D
from q1D import Q1D


def eo(_):
    return lambda geom_info, cbflags, name: EO(geom_info, cbflags, name)


def qper(nparts):
    return lambda geom_info, dimension, name: Q1D(geom_info, dimension, name,
                                                  nparts, "PERIODIC")


def qopen(nparts):
    return lambda geom_info, dimension, name: Q1D(geom_info, dimension, name,
                                                  nparts, "OPEN")


def hbb(halo):
    return lambda geom_info, dimension, name: HBB1D(geom_info, dimension, name,
                                                    halo)


def leaf(_):
    return lambda geom_info, dimension, name: Leaf1D(geom_info, dimension, name
                                                     )


def end(_):
    return lambda geom_info, dimension, name: None


partitioners_dict = {
    "qper": qper,
    "qopen": qopen,
    "hbb": hbb,
    "eo": eo,
    "leaf": leaf,
    "end":end,
}
