from pathlib import Path
import sys

import pytest


REPO_ROOT = Path(__file__).resolve().parents[2]
if str(REPO_ROOT) not in sys.path:
    sys.path.insert(0, str(REPO_ROOT))

from library.library import Array


@pytest.mark.parametrize("size", [0, 1, 8])
def test_constructor_and_size(size: int) -> None:
    arr = Array(size)

    assert len(arr) == size
    assert arr.size() == size


@pytest.mark.parametrize(
    ("size", "factor"),
    [
        (1, 10),
        (5, 3),
        (10, -2),
    ],
)
def test_element_access(size: int, factor: int) -> None:
    arr = Array(size)

    for idx in range(size):
        arr[idx] = idx * factor

    for idx in range(size):
        expected = idx * factor
        assert arr[idx] == expected
        assert arr.at(idx) == expected


@pytest.mark.parametrize("size", [1, 5, 10])
def test_copy_is_deep_copy(size: int) -> None:
    original = Array(size)

    for idx in range(size):
        original[idx] = idx + 100

    copied = original.copy()

    for idx in range(size):
        assert copied[idx] == original[idx]

    copied[0] = -999
    assert original[0] == 100


@pytest.mark.parametrize(
    ("initial_size", "resized"),
    [
        (0, 1),
        (4, 0),
        (10, 3),
    ],
)
def test_allocate_and_clear(initial_size: int, resized: int) -> None:
    arr = Array(initial_size)
    assert arr.size() == initial_size

    arr.allocate(resized)
    assert arr.size() == resized

    if resized > 0:
        arr[0] = 42
        assert arr[0] == 42

    arr.clear()
    assert arr.size() == 0


@pytest.mark.parametrize(
    ("size", "index"),
    [
        (0, 0),
        (1, -1),
        (1, 1),
        (3, -4),
        (3, 3),
    ],
)
def test_invalid_index_raises_index_error(size: int, index: int) -> None:
    arr = Array(size)

    with pytest.raises(IndexError):
        _ = arr[index]

    with pytest.raises(IndexError):
        arr[index] = 123

    with pytest.raises(IndexError):
        _ = arr.at(index)
