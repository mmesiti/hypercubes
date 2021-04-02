# coding: utf-8
import eo
from hypothesis import given, settings
from hypothesis import strategies as st


@given(st.lists(st.integers(min_value=1, max_value=10), min_size=4,
                max_size=4))
def test_coord_idx_conversion_pair(sizes):
    cumsizes = eo.get_cumsizes(sizes)
    for i in range(sizes[0]):
        for j in range(sizes[1]):
            for k in range(sizes[2]):
                for m in range(sizes[3]):
                    coords = [i, j, k, m]
                    assert coords == eo.lex_idx_to_coord(
                        eo.lex_coord_to_idx(coords, cumsizes), sizes)


def count_eo(sizes):
    '''
    Count number of even and odd sites
    in a hypercubic lattice of given size.
    '''
    counts = [0, 0]
    max_idx = eo.get_cumsizes(sizes)[-1]
    for i in range(max_idx):
        coords = eo.lex_idx_to_coord(i, sizes)
        parity = sum(coords) % 2
        counts[parity] += 1
    return counts


@settings(deadline=800)
@given(st.lists(st.integers(min_value=1, max_value=10), min_size=1,
                max_size=6))
def test_e_ge_o(sizes):
    '''
    Test that
    the number of even sites
    (actually, the number of sites
    having same parity as the origin)
    is greater than or equal to
    the number of odd sites
    (actually, the number of sites
    having opposite parity compared to the origin)
    '''
    e, o = count_eo(sizes)
    assert e >= o
