#!/usr/bin/env python3
from bool_maybe import BoolM


def test_and_symmetric():
    for a in [BoolM.F, BoolM.T, BoolM.M]:
        for b in [BoolM.F, BoolM.T, BoolM.M]:
            assert (a & b) == (b & a)

def test_or_symmetric():
    for a in [BoolM.F, BoolM.T, BoolM.M]:
        for b in [BoolM.F, BoolM.T, BoolM.M]:
            assert (a | b) == (b | a)
