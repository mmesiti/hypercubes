#!/usr/bin/env python3
from partitioning import Partitioning


class Partitioning1D(Partitioning):
    def __init__(self, geom_info, comments):
        pass

    @property
    def starts(self):
        return self.limits[:-1]

    @property
    def ends(self):
        return self.limits[1:]

    def sub_geom_info_list(self):
        parities = [(self.parity + s) % 2 for s in self.starts]
        subsizes = [e - s for s, e in zip(self.starts, self.ends)]

        all_kinds = set(
            (subsize, parity) for subsize, parity in zip(subsizes, parities))

        return list(all_kinds)

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
        def _idx_to_subsize(idx, starts, ends):
            return ends[idx] - starts[idx]

        def _idx_to_start_parity(idx, starts, parity):
            return (starts[idx] + parity) % 2

        for i, (subsize, parity) in enumerate(self.sub_geom_info_list()):
            subsize_ = _idx_to_subsize(idx, self.starts, self.ends)
            parity_ = _idx_to_start_parity(idx, self.starts, parity)
            if (subsize_, parity_) == (subsize, parity):
                return i

    def max_idx_value(self):
        pass
