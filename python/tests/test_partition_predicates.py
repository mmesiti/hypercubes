#!/usr/bin/env python3
import fixtures
import partition_predicates as pps

import pytest
from hypothesis import given, settings, HealthCheck, assume
from hypothesis import strategies as st


@pytest.fixture
def p4D42():
    return fixtures.partitioners_list_4D_42()


@settings(suppress_health_check=[HealthCheck.function_scoped_fixture])
@given(idhbb=st.lists(st.integers(min_value=0, max_value=4),
                      min_size=4,
                      max_size=4))
def test_only_bulk_and_borders(idhbb, p4D42):
    res = not any(x in {0, 4} for x in idhbb)
    idx = [
        2,
        3,
        1,
        0,  #
        1,
        2,
        1,
        1,  #
        *idhbb,  #
        0,
        0
    ]  #

    assert pps.get_NmD_halo_predicate(0)(p4D42, idx) == res


@settings(suppress_health_check=[HealthCheck.function_scoped_fixture])
@given(idhbb=st.lists(st.integers(min_value=0, max_value=4),
                      min_size=4,
                      max_size=4),
       D=st.integers(min_value=1, max_value=4))
def test_only_bulk_and_borders(idhbb, D, p4D42):
    halo_dimensionality = sum(x in {0, 4} for x in idhbb)
    idx = [
        2,
        3,
        1,
        0,  #
        1,
        2,
        1,
        1,  #
        *idhbb,  #
        0,
        0
    ]  #

    res = halo_dimensionality <= D
    predicate = pps.get_NmD_halo_predicate(D)
    assert predicate(p4D42, idx) == res


@settings(suppress_health_check=[HealthCheck.function_scoped_fixture])
@given(
    MPI_rank_idx=st.lists(st.integers(min_value=0, max_value=3),
                          min_size=4,
                          max_size=4),
    MPI_rank_selected=st.lists(st.integers(min_value=0, max_value=3),
                               min_size=4,
                               max_size=4),
)
def test_MPI_selection_wrong(MPI_rank_idx, MPI_rank_selected, p4D42):
    assume(MPI_rank_idx != MPI_rank_selected)
    idx_wrong = [
        *MPI_rank_idx,  #
        1,
        2,
        1,
        1,  #
        1,
        2,
        0,
        3,  #
        0,
        0
    ]  #
    predicate = pps.get_mpi_rank_predicate(MPI_rank_selected)
    assert not predicate(p4D42, idx_wrong)


@settings(suppress_health_check=[HealthCheck.function_scoped_fixture])
@given(MPI_rank_idx=st.lists(st.integers(min_value=0, max_value=3),
                             min_size=4,
                             max_size=4))
def test_MPI_selection_right(MPI_rank_idx, p4D42):
    idx_right = [
        *MPI_rank_idx,  #
        1,
        2,
        1,
        1,  #
        1,
        2,
        0,
        3,  #
        0,
        0
    ]  #

    predicate = pps.get_mpi_rank_predicate(MPI_rank_idx)

    assert predicate(p4D42, idx_right)
