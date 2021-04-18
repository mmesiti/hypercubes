#!/usr/bin/env python3
import eo
from hypothesis import given, settings
from hypothesis import strategies as st
from fixtures import nexamples


@settings(max_examples=nexamples(1000))
@given(st.lists(st.integers(min_value=1, max_value=10), min_size=1,
                max_size=6), st.data())
def test_coord_eoidx_conversion_pair(sizes, data):
    cumsizes = eo.get_cumsizes(sizes)
    i = data.draw(st.integers(min_value=0, max_value=cumsizes[-1] - 1))
    coords = eo.lex_idx_to_coord(i, sizes)
    eoflag, idxh = eo.lex_coord_to_eoidx(coords, cumsizes)
    coords2 = eo.lexeo_idx_to_coord(eoflag, idxh, sizes)
    info = dict(eoflag=eoflag, idxh=idxh, i=i, coords=coords, sizes=sizes)
    assert coords2 == coords, info


@settings(max_examples=nexamples(1000))
@given(sizes=st.lists(st.integers(min_value=1, max_value=10),
                      min_size=1,
                      max_size=6),
       data=st.data())
def test_coord_eoidx_conversion_pair_2(sizes, data):
    cumsizes = eo.get_cumsizes(sizes)
    coords = [data.draw(st.integers(0, s - 1)) for s in sizes]
    eoflag, idxh = eo.lex_coord_to_eoidx(coords, cumsizes)
    coords2 = eo.lexeo_idx_to_coord(eoflag, idxh, sizes)
    info = dict(eoflag=eoflag, idxh=idxh, coords=coords, sizes=sizes)
    assert coords2 == coords, info
