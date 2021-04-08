#!/usr/bin/env python3
'''
The way that cumsize is computed
needs to match the way the other functions work.
In particular,
lexeo_idx_to_coord needs to recover the right parity
by adding 1 to the "fastest varying" coordinate
used in computing cumsize.

NOTE: all dimensions are assumed to be checkerboarded.
'''


def get_cumsizes(sizes):
    cumsizes = [1]
    for size in sizes:
        cumsizes.append(size * cumsizes[-1])

    return cumsizes


def lex_coord_to_idx(relative_xs, cumsizes):
    return sum(relative_x * cumsize  #
               for relative_x, cumsize  #
               in zip(relative_xs, cumsizes))


def coord_to_eo(relative_xs):
    return sum(relative_xs) % 2


def lex_coord_to_eoidx(relative_xs, cumsizes):
    idxh = lex_coord_to_idx(relative_xs, cumsizes) // 2
    eo = coord_to_eo(relative_xs)

    return eo, idxh


def lex_idx_to_coord(idx, sizes):
    coordinates = []
    i = idx
    for s in sizes:
        coordinates.append(i % s)
        i = i // s
    return coordinates


def lexeo_idx_to_coord(eo, idxh, sizes):
    coord = lex_idx_to_coord(2 * idxh, sizes)
    if eo == coord_to_eo(coord):
        return coord
    else:
        return lex_idx_to_coord(2 * idxh + 1, sizes)
