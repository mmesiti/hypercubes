#!/usr/bin/env python3

from enum import IntEnum


class BoolM(IntEnum):
    # Just for convenience,
    # not really meaningful
    F = 0
    T = 1
    M = 2  # Maybe

    def __and__(self, other):
        and_truth_table = [
            [self.F, self.F, self.F],  #
            [self.F, self.T, self.M],  #
            [self.F, self.M, self.M]
        ]
        return and_truth_table[self][other]

    def __or__(self, other):
        or_truth_table = [
            [self.F, self.T, self.M],  #
            [self.T, self.T, self.T],  #
            [self.M, self.T, self.M]
        ]
        return or_truth_table[self][other]

    def __invert__(self):
        not_truth_table = [self.T, self.F, self.M]
        return not_truth_table[self]
