#!/usr/bin/env python3
from collections import namedtuple

IndexResult = namedtuple("IndexResult", ["idx", "rest", "cached_flag"])


class Partitioning:
    def __init__(self):
        pass

    def sub_geom_info_list(self):
        pass

    def coord_to_idxs(self, relative_x):
        '''
        Returns a number of idxs
        representing the partition
        where relative_x can be.
        "Ghost" copies (halos)
        are included.
        '''
        pass

    def idx_to_coords(self, idx, offset):
        '''
        Returns the relative_x
        given the idx of the partition.
        '''
        pass

    def idx_to_sizes(self, idx, sizes):
        '''
        Return the sizes of the partition indexed by idx.
        '''
        pass

    def idx_to_child_kind(self, idx):
        pass

    def max_idx_value(self):
        pass
