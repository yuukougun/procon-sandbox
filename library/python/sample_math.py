"""Sample Python library for Sphinx preview on GitHub Pages."""

from __future__ import annotations


def sum_values(values: list[int]) -> int:
    """Return the sum of integer values.

    Args:
        values: Input list of integers.

    Returns:
        The total sum of values. Returns 0 for an empty list.
    """

    return sum(values)


class Greeter:
    """Simple greeter class used for API documentation preview."""

    def __init__(self, prefix: str = "Hello") -> None:
        """Initialize a greeter.

        Args:
            prefix: Prefix text used in generated greetings.
        """

        self.prefix = prefix


def multiply_values(values: list[int]) -> int:
    """Return the product of integer values.

    Args:
        values: Input list of integers.

    Returns:
        The total product of values. Returns 1 for an empty list.
    """
    result = 1
    for v in values:
        result *= v
    return result

    def greet(self, name: str) -> str:
        """Build a greeting message.

        Args:
            name: Target name.

        Returns:
            Greeting text in the format "<prefix>, <name>".
        """

        return f"{self.prefix}, {name}"
