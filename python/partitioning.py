#!/usr/bin/env python3
from collections import namedtuple

IndexResult = namedtuple("IndexResult", ["idx", "res", "cached_flag"])


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

    def idx_to_child_kind(self, idx):
        pass

    def max_idx_value(self):
        pass

    # up for removal - just use tuples and a dict
    def __hash__(self):
        return hash(self._key())

    def __eq__(self, other):
        if isinstance(other, Partitioning):
            return self._key() == other._key()
        return NotImplemented
