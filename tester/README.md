# Tester

`library` 配下の C++ / Python ライブラリの回帰テストを管理します。

## テスト配置

- Python: `tester/python/unit/library/test_array.py` (pytest)
- C++: `tester/cpp/unit/library/array_test.cpp` (Google Test)

両言語とも、1つのクラス・関数を複数パラメータで検証するパラメータ化テストを基本とします。

## ローカル実行

- C++ テスト: `make test-cpp`
- Python テスト: `make test-py`
- 全体テスト: `make test`
- ラッパー実行: `python3 tester/testAll.py`

## VS Code Testing View での確認

1. Python 拡張 (`ms-python.python`) をインストールする。
2. CMake Tools 拡張 (`ms-vscode.cmake-tools`) をインストールする。
3. コマンドパレットで `Python: Discover Tests` を実行する。
4. コマンドパレットで `CMake: Configure` → `CMake: Build` を実行する。
5. Testing View で pytest と CTest のテストを実行する。