from __future__ import annotations

"""pytest 共通初期化。

このファイルは、テスト実行時に import 解決しやすいよう sys.path を補正する。
プロジェクト直下のモジュールを安定して参照できることを目的としている。
"""

import os
import sys
from pathlib import Path


# tester/python/conftest.py から2階層上 = リポジトリルート。
REPO_ROOT = Path(__file__).resolve().parents[2]

# 追加の import ルートを受け取る環境変数名。
# 例: TESTER_EXTRA_PYTHONPATH="ai/python:library/python"
EXTRA_IMPORT_ENV = "TESTER_EXTRA_PYTHONPATH"


def _iter_import_roots() -> list[Path]:
    """sys.path に追加する候補ディレクトリを列挙して返す。

    ルール:
    1. 既定で REPO_ROOT は常に候補に含める。
    2. TESTER_EXTRA_PYTHONPATH があれば os.pathsep 区切りで読む。
    3. 相対パス指定は REPO_ROOT 基準の絶対パスへ変換する。
    4. 実在するパスのみを返す。
    """

    # まずは既定の import ルートを入れる。
    roots: list[Path] = [
        REPO_ROOT,
    ]

    # 環境変数から追加ルートを読み取る。未設定時は空文字。
    raw_extra_paths = os.environ.get(EXTRA_IMPORT_ENV, "")
    if raw_extra_paths:
        # Linux/macOS では ':'、Windows では ';' 区切りになる。
        for value in raw_extra_paths.split(os.pathsep):
            entry = value.strip()
            if not entry:
                continue

            # 相対パスはリポジトリルート基準に補正する。
            path = Path(entry)
            if not path.is_absolute():
                path = REPO_ROOT / path
            roots.append(path)

    # 実在しないパスは sys.path に入れない。
    existing_roots = [path.resolve() for path in roots if path.exists()]
    return existing_roots


# 優先度を上げるため先頭挿入する。
# 既に存在するパスは重複追加しない。
for import_root in _iter_import_roots():
    import_root_str = str(import_root)
    if import_root_str not in sys.path:
        sys.path.insert(0, import_root_str)