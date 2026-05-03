<!-- BEGIN-NAVIGATION -->
# 目次 [home](/docs/tester-docs/README.md)
- [1. 前提知識](/docs/tester-docs/01_prerequisites.md) - now
- [2. アーキテクチャ](/docs/tester-docs/02_architecture.md)
- [3. Python pytest](/docs/tester-docs/03_python_pytest.md)
- [4. C++ GoogleTest + C++ TestMate](/docs/tester-docs/04_cpp_gtest_testmate.md)
- [5. ゼロからの実装](/docs/tester-docs/05_implementation_from_scratch.md)
- [6. 運用とCI](/docs/tester-docs/06_operations_ci.md)
- [7. トラブルシューティング](/docs/tester-docs/07_troubleshooting.md)

## 1. 前提知識
<!-- END-NAVIGATION -->

# 01. 前提知識
f
この章では、tester を理解するために必要な基礎を整理します。

## 1. テストの基本用語

### 1.1 テストとは

- 実装したコードが期待どおりに動くか確認する作業。
- 人手確認ではなく、コードで自動確認するのがこのプロジェクトの方針。

### 1.2 単体テスト (Unit Test)

- 関数やクラスなど、小さい単位で動作を検証するテスト。
- このプロジェクトの tester は単体テストが中心。

### 1.3 回帰テスト (Regression Test)

- 以前は動いていた機能が、変更後も壊れていないことを確認するテスト。
- ライブラリ更新時に既存利用者のコード破壊を防ぐ目的で重要。

### 1.4 アサーション (Assertion)

- 「この値であるべき」という宣言。
- Python では `assert`、GoogleTest では `EXPECT_EQ` などを使う。

### 1.5 パラメータ化テスト

- 同じテスト本体を、複数入力値で繰り返し実行する手法。
- 例:
  - サイズ 0, 1, 8 を同じロジックで検証
  - 正常値・境界値・異常値をまとめて管理

---

## 2. Python 側の前提知識

### 2.1 pytest

- Python の代表的なテストフレームワーク。
- `test_*.py` という命名規則でテストを自動検出する。
- `@pytest.mark.parametrize` でパラメータ化テストを記述できる。

### 2.2 import と PYTHONPATH

- このプロジェクトではライブラリが `library/` 以外へ増えることを想定している。
- テストから import するために、以下を使う:
  - 共通設定ファイル `tester/python/conftest.py`
  - 必要に応じて `TESTER_EXTRA_PYTHONPATH` 環境変数

`conftest.py` はまずリポジトリルートを import パスへ追加し、必要分だけ追加パスを環境変数で拡張する設計です。

---

## 3. C++ 側の前提知識

### 3.1 GoogleTest (gtest)

- C++ の標準的な単体テストフレームワーク。
- 基本マクロ:
  - `TEST` / `TEST_F` / `TEST_P`
  - `EXPECT_EQ`, `EXPECT_TRUE`, `EXPECT_DEATH` など

### 3.2 CMake

- C++ のビルド設定を記述するためのツール。
- このプロジェクトでは CMake で gtest を取得し、階層化された `*_test.cpp` を自動収集してテストバイナリを生成する。
- 追加 include パスは `TESTER_CPP_EXTRA_INCLUDE_DIRS` で拡張できる。

### 3.3 C++ TestMate

- VS Code で gtest をテスト一覧として可視化・実行する拡張。
- 役割は「テストバイナリを見つけて実行」。
- テストバイナリを作るのは CMake 側の担当。

---

## 4. VS Code 側の前提知識

必要拡張:

- Python: `ms-python.python`
- C/C++: `ms-vscode.cpptools`
- CMake Tools: `ms-vscode.cmake-tools`
- C++ TestMate: `matepek.vscode-catch2-test-adapter`

Testing View の内訳:

- Python テスト: Python 拡張が pytest を検出
- C++ テスト: C++ TestMate が gtest バイナリを検出

---

## 5. コマンドライン前提

最低限覚えるコマンド:

```bash
# プロジェクトルートへ移動
cd /workspaces/procon-sandbox

# Python テスト
make test-py

# C++ テスト
make test-cpp

# 両方
make test
```

エラー時の基本姿勢:

1. エラーメッセージを先頭から読む
2. 最初の 1 件の根本原因に集中する
3. 再実行して次のエラーへ進む

---

## 6. この章の完了条件

以下を説明できれば次章へ進んで良いです。

- 単体テスト / 回帰テスト / パラメータ化テストの違い
- pytest と gtest の役割
- CMake と C++ TestMate の役割分担
- `make test-py`, `make test-cpp`, `make test` の意味
