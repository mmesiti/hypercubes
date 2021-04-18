#!/usr/bin/env python3
import tree
import pytest


def test_collapse_level1():
    t = (1, (
        (2, (
            (4, ()),
            (5, ()),
        )),
        (3, ((6, ()), (7, ()))),
    ))

    res = tree.collapse_level(t, 1, 0)

    texp = (1, (
        (4, ()),
        (6, ()),
    ))

    assert res == texp


def test_collapse_level_none():
    '''
    Cannot collapse leaves, actually.
    '''
    t = (0, (
        (
            1,
            ((None, ()), ),
        ),
        (
            2,
            ((None, ()), ),
        ),
    ))
    s = (0, ((None, ()), (None, ())))

    res = tree.collapse_level(t, 1, 0)
    assert s == res


def test_bring_level_on_top_noop():
    '''
    Bringing on top level 0 is a no op.
    '''
    t = (1, (
        (2, (
            (4, ()),
            (5, ()),
        )),
        (3, ((6, ()), (7, ()))),
    ))

    res = tree.bring_level_on_top(t, 0)
    assert res == t


def test_bring_level_on_top_mini():
    '''
    NOTE: Cannot bring leaves on top.
    '''
    t = (0, (
        (1, ((None, ()), )),
        (1, ((None, ()), )),
    ))
    s = (1, ((0, ((None, ()), (None, ()))), ))

    res = tree.bring_level_on_top(t, 1)
    assert res == s


def test_bring_level_on_top():
    t = (1, (
        (2, (
            (4, ()),
            (5, ()),
        )),
        (2, ((6, ()), (7, ()))),
    ))

    res = tree.bring_level_on_top(t, 1)

    texp = (2, (
        (1, (
            (4, ()),
            (6, ()),
        )),
        (1, ((5, ()), (7, ()))),
    ))

    assert texp == res


def test_bring_level_on_top_asymmetric():
    t = (0, ((1, (
        (2, (
            (4, ()),
            (5, ()),
        )),
        (2, ((6, ()), (7, ()))),
    )), (1, ((2, ((6, ()), (7, ()))), ))))

    res = tree.bring_level_on_top(t, 2)

    expt = (2, (
        (0, (
            (1, (
                (4, ()),
                (6, ()),
            )),
            (1, ((6, ()), )),
        )),
        (0, (
            (1, (
                (5, ()),
                (7, ()),
            )),
            (1, ((7, ()), )),
        )),
    ))

    assert expt == res


def test_swap_levels_noop():
    t = (1, (
        (2, (
            (4, ()),
            (5, ()),
        )),
        (2, ((6, ()), (7, ()))),
    ))
    new_level_ordering = [0, 1]

    res = tree.swap_levels(t, new_level_ordering)
    assert res == t


def test_swap_levels():
    t = (1, (
        (2, (
            (4, ()),
            (5, ()),
        )),
        (2, ((6, ()), (7, ()))),
    ))
    new_level_ordering = [1, 0]
    res = tree.swap_levels(t, new_level_ordering)

    expt = (2, (
        (1, (
            (4, ()),
            (6, ()),
        )),
        (1, ((5, ()), (7, ()))),
    ))
    assert res == expt


def test_swap_levels_list_noop():
    t = (0, ((1, ((2, ((3, ((4, ((5, ()), )), )), )), )), ))
    new_level_ordering = [0, 1, 2, 3, 4]
    res = tree.swap_levels(t, new_level_ordering)
    assert t == res


def test_swap_levels_list():
    t = (0, ((1, ((2, ((3, ((4, ((5, ()), )), )), )), )), ))
    new_level_ordering = [2, 1, 0, 4, 3]
    res = tree.swap_levels(t, new_level_ordering)
    expt = (2, ((1, ((0, ((4, ((3, ((5, ()), )), )), )), )), ))
    assert res == expt


def complex_example():
    t = (
        3,
        (  #
            (
                4,
                (  #
                    (7, ()),
                    (9, ()),
                )),
            (
                6,
                (  #
                    (
                        5,
                        (  #
                            (
                                8,
                                (  #
                                    (4, ()), )), )),
                    (6, ()),  #
                    (8, ())))))  #
