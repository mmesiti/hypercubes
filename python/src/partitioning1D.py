#!/usr/bin/env python3
from partitioning import Partitioning


class Partitioning1D(Partitioning):
    @property
    def starts(self):
        return self.limits[:-1]

    @property
    def ends(self):
        return self.limits[1:]

    def sub_geom_info_list(self):
        def partition_parity(start):
            if self.parity is not None:
                return (self.parity + start) % 2
            else:
                return None

        parities = [partition_parity(s) for s in self.starts]
        subsizes = [e - s for s, e in zip(self.starts, self.ends)]

        all_kinds = set(
            (subsize, parity) for subsize, parity in zip(subsizes, parities))

        return tuple(all_kinds)

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

        subsize_ = _idx_to_subsize(idx, self.starts, self.ends)
        parity_ = _idx_to_start_parity(idx, self.starts, self.parity)
        for i, (subsize, parity) in enumerate(self.sub_geom_info_list()):
            if (subsize_, parity_) == (subsize, parity):
                return i

    def max_idx_value(self):
        pass
