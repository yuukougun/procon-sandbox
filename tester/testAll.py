#!/usr/bin/env python3

from __future__ import annotations

import subprocess
import sys
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
TARGETS = ("test-cpp", "test-py")


def run_target(target: str) -> int:
    print(f"[RUN] make {target}")
    completed = subprocess.run(["make", target], cwd=REPO_ROOT)
    return completed.returncode


def main() -> int:
    for target in TARGETS:
        code = run_target(target)
        if code != 0:
            print(f"[FAIL] make {target} (exit={code})")
            return code

    print("[OK] all tests passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
