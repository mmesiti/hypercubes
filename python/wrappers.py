#!/usr/bin/env python3
from box import Box


def dimensionalize(monodim_partitioner, geom_infos, dimension, *args,
                   **kwargs):
    '''
    Takes a 1D partitioner and behaves as a multiple dimension one
    '''
    geom_info = geom_infos[dimension]
    monodim_partitioner_output = monodim_partitioner(geom_info, *args,
                                                     **kwargs)

    new_geom_info_list = [[
        new_geom_info if d == dimension else gi
        for d, gi in enumerate(geom_infos)
    ] for new_geom_info in monodim_partitioner_output.new_geom_info_list]

    def dimensionalized_coord_to_idxs(relative_xs):
        possible_indices = monodim_partitioner_output.coord_to_idxs(
            relative_xs[dimension])

        def dimensionalize_rest(value):
            if type(value) == str:
                import pdb
                pdb.set_trace()

            rests = [
                relative_x if i != dimension else value.rest
                for i, relative_x in enumerate(relative_xs)
            ]
            return Box(idx=value.idx,
                       rests=rests,
                       cached_flag=value.cached_flag)

        return [dimensionalize_rest(i) for i in possible_indices]

    def dimensionalized_idx_to_coord(idx, offsets):
        coordinates = list(offsets)
        coordinates[dimension] = monodim_partitioner_output.idx_to_coord(idx)
        return coordinates

    return Box(new_geom_info_list=new_geom_info_list,
               coords_to_idxs=dimensionalized_coord_to_idxs,
               idx_to_coords=dimensionalized_idx_to_coord,
               idx_to_child_kind=monodim_partitioner_output.idx_to_child_kind,
               comments=monodim_partitioner_output.comments +
               f" Geom info: {geom_info}, Dimension: {dimension}")  #


def objectify(partitioner, *args, **kwargs):
    return lambda ranges: partitioner(ranges, *args, **kwargs)
