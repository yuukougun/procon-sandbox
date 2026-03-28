"""Python interface for the pybind11 C++ Othello engine."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Any, Dict

def _append_cpp_engine_paths() -> None:
    repo_root = Path(__file__).resolve().parents[2]
    candidates = [
        repo_root,
        repo_root / "ai/python/build",
    ]
    candidates.extend((repo_root / "ai/python/build").glob("lib.*"))

    for p in candidates:
        if p.exists():
            s = str(p)
            if s not in sys.path:
                sys.path.insert(0, s)


try:
    import othello_cpp_engine as _engine
except ModuleNotFoundError:
    _append_cpp_engine_paths()
    import othello_cpp_engine as _engine


def generate_dataset_beam(
    mode: str,
    dataset_path: str,
    *,
    inference_script: str = "ai/python/value_inference_wrapper.py",
    model_path: str = "",
    model_black: str = "",
    model_white: str = "",
    model_side: str = "black",
    beam_width: int = 32,
    beam_top_k: int = 2,
    tie_break: str = "fixed",
    log_interval: int = 1,
    seed: int = 42,
) -> Dict[str, Any]:
    """Run one beam-search game and append records to dataset.bin.

    mode: random | guided | duel
    """

    return dict(
        _engine.generate_dataset_beam(
            mode=mode,
            dataset_path=dataset_path,
            inference_script=inference_script,
            model_path=model_path,
            model_black=model_black,
            model_white=model_white,
            model_side=model_side,
            beam_width=beam_width,
            beam_top_k=beam_top_k,
            tie_break=tie_break,
            log_interval=log_interval,
            seed=seed,
        )
    )


def play_duel_once(
    *,
    model_black: str,
    model_white: str,
    inference_script: str = "ai/python/value_inference_wrapper.py",
    tie_break: str = "fixed",
    seed: int = 42,
) -> int:
    """Run one duel game and return result.

    Return value: 1 (black win), 0 (draw), -1 (white win)
    """

    return int(
        _engine.play_duel_once(
            model_black=model_black,
            model_white=model_white,
            inference_script=inference_script,
            tie_break=tie_break,
            seed=seed,
        )
    )
