#!/usr/bin/env python3
import eo
from hypothesis import given, settings
from hypothesis import strategies as st


@settings(max_examples=1000)
@given(st.lists(st.integers(min_value=1, max_value=10), min_size=1,
                max_size=6), st.data())
def test_coord_eoidx_conversion_pair(sizes, data):
    cumsizes = eo.get_cumsizes(sizes)
    i = data.draw(st.integers(min_value=0, max_value=cumsizes[-1] - 1))
    coords = eo.lex_idx_to_coord(i, sizes)
    eoflag, idxh = eo.lex_coord_to_eoidx(coords, cumsizes)
    coords2 = eo.lexeo_idx_to_coord(eoflag, idxh, sizes)
    info = dict(eoflag=eoflag,
                idxh=idxh,
                i=i,
                coords=coords,
                sizes=sizes)
    assert coords2 == coords, info
