# Tester

このフォルダは、C++ / Python ライブラリの回帰テストを管理します。

`library` フォルダに限らず、将来的に他フォルダへ増えるライブラリもテストできる構成です。

## 詳細ドキュメント

初心者向けに、前提知識から実装・運用・トラブルシュートまでをまとめています。

- 入口: `tester/docs/README.md`
- 前提知識: `tester/docs/01_prerequisites.md`
- 全体構成: `tester/docs/02_architecture.md`
- Python 実装: `tester/docs/03_python_pytest.md`
- C++ 実装: `tester/docs/04_cpp_gtest_testmate.md`
- ゼロから実装: `tester/docs/05_implementation_from_scratch.md`
- 運用と CI: `tester/docs/06_operations_ci.md`
- 障害対応: `tester/docs/07_troubleshooting.md`

## テスト配置

- Python: `tester/python/unit/library/test_array.py` (pytest)
- C++: `tester/cpp/unit/library/array_test.cpp` (Google Test)

推奨の階層テンプレート:

```text
tester/
├── python/
│   ├── conftest.py
│   ├── unit/
│   │   ├── library/
│   │   ├── ai/
│   │   └── python/
│   └── integration/
│       ├── library/
│       └── ai/
└── cpp/
	├── CMakeLists.txt
	├── unit/
	│   ├── library/
	│   ├── ai/
	│   └── python/
	└── integration/
		├── library/
		└── ai/
```

命名の目安:

- Python: `test_*.py`
- C++: `*_test.cpp`

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
- 追加 include が必要なら環境変数 `TESTER_CPP_EXTRA_INCLUDE_DIRS` を使います。

例:

```bash
TESTER_CPP_EXTRA_INCLUDE_DIRS="/workspaces/procon-sandbox/ai/cpp/include:/workspaces/procon-sandbox/custom/include" make test-cpp
```

## VS Code Testing View での確認

1. Python 拡張 (`ms-python.python`) をインストールする。
2. C++ TestMate 拡張 (`matepek.vscode-catch2-test-adapter`) をインストールする。
3. CMake Tools 拡張 (`ms-vscode.cmake-tools`) をインストールする。
4. コマンドパレットで `Python: Discover Tests` を実行する。
5. コマンドパレットで `CMake: Configure` → `CMake: Build` を実行する。
6. Testing View で pytest と C++ TestMate のテストを実行する。