<!-- BEGIN-NAVIGATION -->
# 目次 [home](/docs/tester-docs/README.md)
- [1. 前提知識](/docs/tester-docs/01_prerequisites.md)
- [2. アーキテクチャ](/docs/tester-docs/02_architecture.md)
- [3. Python pytest](/docs/tester-docs/03_python_pytest.md) - now
- [4. C++ GoogleTest + C++ TestMate](/docs/tester-docs/04_cpp_gtest_testmate.md)
- [5. ゼロからの実装](/docs/tester-docs/05_implementation_from_scratch.md)
- [6. 運用とCI](/docs/tester-docs/06_operations_ci.md)
- [7. トラブルシューティング](/docs/tester-docs/07_troubleshooting.md)

## 3. Python pytest
<!-- END-NAVIGATION -->

# 03. Python テスト (pytest)

この章では、Python テスターを追加・実行・運用する流れを説明します。

## 1. 対象ファイル

- 実装対象（例）: `library/library.py`
- テストファイル（例）: `tester/python/unit/library/test_array.py`
- 共通設定: `tester/python/conftest.py`

---

## 2. 現在の import 方式

`tester/python/conftest.py` が pytest 起動時に import ルートを設定します。

基本方針:

- まずリポジトリルートを `sys.path` へ追加
- `TESTER_EXTRA_PYTHONPATH` で追加ルートを拡張

例:

```bash
export TESTER_EXTRA_PYTHONPATH="ai/python:python"
make test-py
```

この設計により、将来 `library/` 以外に実装が増えても、テスト側を大きく書き換えずに対応できます。

---

## 3. テストファイルの基本形

```python
import pytest
from library.library import Array


def test_example():
    arr = Array(3, 0)
    assert arr[0] == 0
```

命名規則:

- ファイル名: `test_*.py`
- 関数名: `test_*`

---

## 4. パラメータ化テスト

同じロジックを複数条件で確認する場合:

```python
@pytest.mark.parametrize("size", [1, 2, 8])
def test_array_size(size: int):
    arr = Array(size, 0)
    assert len(arr) == size
```

利点:

- ケース追加が簡単
- 失敗条件が明確
- テスト重複を減らせる

---

## 5. 例外テスト

境界外アクセスなどの異常系は `pytest.raises` で確認します。

```python
def test_out_of_bounds_raises() -> None:
    arr = Array(2, 7)
    with pytest.raises(IndexError):
        _ = arr[2]
```

---

## 6. 実行方法

### 6.1 CLI

```bash
make test-py
```

### 6.2 VS Code Testing View

1. Python 拡張が有効な状態でワークスペースを開く
2. Testing View を開く
3. Python テストツリーから対象ケースを実行

---

## 7. 新しい Python テスト追加手順

例: `ai/` 配下の Python 実装をテスト対象に追加する場合

1. テスト配置先を作る

```text
tester/python/unit/ai/
```

2. `test_xxx.py` を追加

3. 必要なら import ルートを環境変数で追加

```bash
export TESTER_EXTRA_PYTHONPATH="ai"
```

4. `make test-py` で実行

5. Testing View で個別ケース実行

---

## 8. よくある失敗

### 8.1 ModuleNotFoundError

原因:

- import ルートが不足
- 実装ファイル構成と import 文が不一致

対処:

- まず `conftest.py` のルート設定を確認
- 次に `TESTER_EXTRA_PYTHONPATH` を必要最小限で追加

### 8.2 テストが検出されない

原因:

- ファイル名が `test_*.py` になっていない
- 関数名が `test_` で始まっていない

---

## 9. この章の完了条件

- 新規 Python テストを 1 ファイル追加できる
- CLI と Testing View の両方で実行できる
- import エラーを自力で切り分けできる
