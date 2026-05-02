# Tester

このフォルダは、C++ / Python ライブラリの回帰テストを管理します。

`library` フォルダに限らず、将来的に他フォルダへ増えるライブラリもテストできる構成です。

## テスト配置

- Python: `tester/python/unit/library/test_array.py` (pytest)
- Python: `tester/python/unit/library/test_sample_math.py` (pytest)
- C++: `tester/cpp/unit/library/array_test.cpp` (Google Test)
- C++: `tester/cpp/unit/library/sample_math_test.cpp` (Google Test)

推奨の階層テンプレート:

```text
tester/
├── python/
│   ├── conftest.py
│   └── unit/
│       ├── library/
│       ├── ai/
│       └── python/
└── cpp/
	├── CMakeLists.txt
	└── unit/
	    ├── library/
	    ├── ai/
	    └── python/
```

命名の目安:

- Python: `test_*.py`
- C++: `*_test.cpp`

## ファイル設定の概要

### Python: `tester/python/conftest.py`

pytest の初期化ファイル。リポジトリルートを sys.path に加えて、import を簡単にします。

役割:
- リポジトリルート直下のモジュール（`library.python` など）を import できるようにする
- 環境変数 `TESTER_EXTRA_PYTHONPATH` で追加の import パスを指定可能

ファイル内の詳細は docstring とコメントを参照してください。

### C++: `tester/cpp/CMakeLists.txt`

GoogleTest と individual test executable 生成のセットアップ。

役割:
- `unit/*_test.cpp` を見つけてテストターゲットを生成
- テストファイル1ファイル = 実行ファイル1つ
- ライブラリ実装(.cpp)は CMake では直接リンクしない
- テストソースが必要な .cpp を直接 `#include "library/cpp/xxx.cpp"` で取り込む

ファイル内の詳細は CMake コメントを参照してください。

## ローカル実行

- C++ テスト: `make test-cpp`
- Python テスト: `make test-py`
- 全体テスト: `make test`
- ラッパー実行: `python3 tester/testAll.py`

## ライブラリ配置が増えた場合の対応

Python:

- `tester/python/conftest.py` がリポジトリルートを import パスに追加します。
- 追加の import ルートが必要なら環境変数 `TESTER_EXTRA_PYTHONPATH` を使います。

例:

```bash
TESTER_EXTRA_PYTHONPATH="ai:python/custom_lib" make test-py
```

C++:

- `tester/cpp/CMakeLists.txt` で `PROJECT_ROOT` と `PROJECT_ROOT/library` を include 対象にします。
- テストソースが実装ファイル(.cpp)を必要とする場合は、テストファイル内で `#include "library/cpp/xxx.cpp"` として直接取り込みます。

## VS Code Testing View での確認

1. Python 拡張 (`ms-python.python`) をインストールする。
2. CMake Tools 拡張 (`ms-vscode.cmake-tools`) をインストールする。
3. コマンドパレットで `Python: Discover Tests` を実行する。
4. コマンドパレットで `CMake: Configure` を実行し、`CMake: Build` でビルドする。
5. Testing View で pytest と CTest の結果を確認できます。