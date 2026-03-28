"""Build script for pybind11 C++ engine module.

Usage:
  python ai/python/build_cpp_engine.py build_ext --inplace
"""

from __future__ import annotations

from pathlib import Path

from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension, build_ext


ROOT = Path(__file__).resolve().parents[2]

ext_modules = [
    Pybind11Extension(
        "othello_cpp_engine",
        [
            str(ROOT / "ai/cpp/src/pybind_module.cpp"),
            str(ROOT / "ai/cpp/src/BitBoard.cpp"),
            str(ROOT / "ai/cpp/src/SelfPlay.cpp"),
        ],
        include_dirs=[str(ROOT / "ai/cpp/include")],
        cxx_std=17,
    )
]


setup(
    name="othello_cpp_engine",
    version="0.1.0",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    options={"build": {"build_base": str(ROOT / "ai/python/build")}},
)
