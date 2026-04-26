import pytest

from library.python.sample_math import Greeter, multiply_values, sum_values


@pytest.mark.parametrize(
    ("values", "expected"),
    [
        ([], 0),
        ([1], 1),
        ([1, 2, 3, 4], 10),
        ([-5, 2, 10], 7),
    ],
)
def test_sum_values_returns_expected_total(values: list[int], expected: int) -> None:
    assert sum_values(values) == expected


@pytest.mark.parametrize(
    ("values", "expected"),
    [
        ([], 1),
        ([7], 7),
        ([2, 3, 4], 24),
        ([-1, 2, -3], 6),
    ],
)
def test_multiply_values_returns_expected_total(values: list[int], expected: int) -> None:
    assert multiply_values(values) == expected


@pytest.mark.parametrize(
    ("kwargs", "expected_prefix"),
    [
        ({}, "Hello"),
        ({"prefix": "Hi"}, "Hi"),
        ({"prefix": ""}, ""),
    ],
)
def test_greeter_stores_prefix(kwargs: dict[str, str], expected_prefix: str) -> None:
    greeter = Greeter(**kwargs)
    assert greeter.prefix == expected_prefix
