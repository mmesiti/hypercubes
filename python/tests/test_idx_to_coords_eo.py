#!/usr/bin/env python3
from eo_partitioning import EO
from hypothesis import strategies as st
from hypothesis import given,settings


@settings(max_examples=1000)
@given(xsl=st.lists(st.integers(),
                   min_size=4,
                    max_size=4),
       sizes=st.lists(st.integers(min_value=1,max_value=50),
                   min_size=4,
                    max_size=4))
def test_to_idx_to_coords_and_back(xsl,sizes):
    xs = tuple(x % s for x, s in zip(xsl,sizes))
    geom_info = [(s, 0) for s in sizes]
    cbflags = (True,)*4
    name = "test"

    eo_partition = EO(geom_info,cbflags,name)

    res = eo_partition.coords_to_idxs(xs)

    idx = res[0].idx
    rest = res[0].rest

    print(idx,rest)
    xss = eo_partition.idx_to_coords(idx,rest)

    assert xs == xss
