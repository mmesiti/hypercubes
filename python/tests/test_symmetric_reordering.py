#!/usr/bin/env python3
import fixtures
import tree


def test_swap_order_symmetric_labelcheck():
    pp = fixtures.symmetric_case()
    new_level_ordering = [3, 2, 1, 0, 8, 9, 10, 11, 12, 13, 4, 5, 6, 7]
    pp2 = tree.swap_levels(pp, new_level_ordering)
    original_level_list = tree.first_nodes_list(pp)
    new_level_list = tree.first_nodes_list(pp2)

    expected_level_list = [
        original_level_list[:-1][i] for i in new_level_ordering
    ]

    assert new_level_list[:-1] == expected_level_list
    assert original_level_list[-1] == new_level_list[-1]
    assert original_level_list[-1] == None
