# 03. Python テスター実装ガイド (pytest)

この章では、Python の tester を追加・保守できるように詳細に説明します。

## 1. 現在のテスト対象

対象実装:

- `library/library.py`
- クラス `Array`

主な仕様:

- `size()` と `__len__()` がサイズを返す
- `allocate()` で再確保
- `clear()` で空配列化
- 添字範囲外は `IndexError`
- `copy()` はディープコピー

---

## 2. pytest の検出ルール

VS Code / pytest は基本的に以下を検出対象にします。

- ファイル名が `test_*.py`
- 関数名が `test_*`

現在ファイル:

- `tester/python/test_array.py`

---

## 3. import 設定の理由

`test_array.py` 冒頭で次を実施しています。

```python
REPO_ROOT = Path(__file__).resolve().parents[2]
if str(REPO_ROOT) not in sys.path:
    sys.path.insert(0, str(REPO_ROOT))
```

理由:

- テストファイルの位置は `tester/python`
- 対象モジュールは `library/library.py`
- 相対 import 崩れを防ぐため、リポジトリルートをパスに追加

実行側でも `PYTHONPATH=.` を併用しているため、CLI / VS Code 双方で安定しやすいです。

---

## 4. 現在のテストケース解説

## 4.1 test_constructor_and_size

- 入力: size = 0, 1, 8
- 観点: コンストラクタの初期サイズ整合

## 4.2 test_element_access

- 入力: (size, factor) を複数組
- 観点: `[]` と `at()` の読み書き整合

## 4.3 test_copy_is_deep_copy

- 観点: コピー後に片方を書き換えても元が変わらないこと

## 4.4 test_allocate_and_clear

- 観点: `allocate()` と `clear()` のサイズ遷移

## 4.5 test_invalid_index_raises_index_error

- 観点: 負数添字・範囲外添字で例外が出ること

---

## 5. パラメータ化テストの書き方

基本形:

```python
@pytest.mark.parametrize("x", [0, 1, 2])
def test_example(x):
    assert x >= 0
```

複数引数:

```python
@pytest.mark.parametrize(
    ("a", "b"),
    [
        (1, 2),
        (3, 4),
    ],
)
def test_example_pair(a, b):
    assert a < b
```

---

## 6. 新しい Python テストを追加する手順

1. `tester/python/test_<対象>.py` を作る
2. `from library.<module> import <symbol>` を書く
3. 正常系、境界値、異常系を分けて `test_*` を作る
4. 重複入力は `@pytest.mark.parametrize` にまとめる
5. `make test-py` で確認

推奨観点:

- 生成直後の初期状態
- 代表値
- 境界値 (0, 1, 最大近傍)
- エラー入力
- 参照共有バグ (ミュータブルなコピー)

---

## 7. 実行コマンド

```bash
# Python テストのみ
make test-py

# 直接 pytest 実行
PYTHONPATH=. pytest -q tester/python

# 失敗詳細を確認
PYTHONPATH=. pytest -vv tester/python
```

---

## 8. よくある失敗と対処

### 8.1 ModuleNotFoundError

- 原因: import パス不足
- 対処:
  - `PYTHONPATH=.` を付ける
  - `sys.path` 追加処理を確認

### 8.2 VS Code でテストが表示されない

- 原因: Python 拡張設定不足
- 対処: `.vscode/settings.json` の pytest 設定確認

### 8.3 例外タイプが違う

- 原因: 実装変更でエラー型が変化
- 対処: 実装仕様に合わせて `pytest.raises` を更新

---

## 9. この章の完了条件

- 1つの Python ライブラリに対して pytest ファイルを新規作成できる
- パラメータ化と例外検証を使い分けできる
- 失敗ログから修正方針を立てられる
