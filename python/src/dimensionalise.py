#!/usr/bin/env python3
from partitioning import IndexResult


def dimensionalise(cls):
    '''
    Decorator that takes a 1D class
    and returns a multi-d capable class.
    '''
    class Dimensionalised:
        def __init__(self, geom_infos, dimension, name, *args, **kwargs):
            self.geom_infos = geom_infos
            self.dimension = dimension
            self.name = name
            geom_info = self.geom_infos[self.dimension]

            pass_args = (geom_info, dimension, name) + args

            self.wrapped = cls(*pass_args, **kwargs)

        def __repr__(self):
            return repr(self.wrapped)

        def __getattr__(self, name):
            return getattr(self.wrapped, name)

        def _merge_in(self, value, list_):
            return tuple(
                value if i == self.dimension else l
                for i, l in enumerate(list_))

        def coords_to_idxs(self, relative_xs):
            relative_x = relative_xs[self.dimension]
            assert type(relative_x) == int
            results1D = self.wrapped.coord_to_idxs(relative_x)

            return tuple(
                IndexResult(
                    idx=r1D.idx,
                    rest=self._merge_in(r1D.rest, relative_xs),
                    cached_flag=r1D.cached_flag,
                ) for r1D in results1D
            )

        def sub_geom_info_list(self):

            all_kinds = self.wrapped.sub_geom_info_list()

            return tuple(self._merge_in(k, self.geom_infos) for k in all_kinds)

        def idx_to_coords(self, idx, offsets):
            offset = offsets[self.dimension]
            coord = self.wrapped.idx_to_coord(idx, offset)

            return self._merge_in(coord, offsets)

    return Dimensionalised
