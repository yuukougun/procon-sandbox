from __future__ import annotations

import os
import sys
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[2]
EXTRA_IMPORT_ENV = "TESTER_EXTRA_PYTHONPATH"


def _iter_import_roots() -> list[Path]:
    roots: list[Path] = [
        REPO_ROOT,
    ]

    raw_extra_paths = os.environ.get(EXTRA_IMPORT_ENV, "")
    if raw_extra_paths:
        for value in raw_extra_paths.split(os.pathsep):
            entry = value.strip()
            if not entry:
                continue
            path = Path(entry)
            if not path.is_absolute():
                path = REPO_ROOT / path
            roots.append(path)

    existing_roots = [path.resolve() for path in roots if path.exists()]
    return existing_roots


for import_root in _iter_import_roots():
    import_root_str = str(import_root)
    if import_root_str not in sys.path:
        sys.path.insert(0, import_root_str)