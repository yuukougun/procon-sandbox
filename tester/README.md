# Tester

`library` 配下の C++ / Python ライブラリの回帰テストを管理します。

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

- Python: `tester/python/test_array.py` (pytest)
- C++: `tester/cpp/array_test.cpp` (Google Test)

## ローカル実行

- C++ テスト: `make test-cpp`
- Python テスト: `make test-py`
- 全体テスト: `make test`
- ラッパー実行: `python3 tester/testAll.py`

## VS Code Testing View での確認

1. Python 拡張 (`ms-python.python`) をインストールする。
2. C++ TestMate 拡張 (`matepek.vscode-catch2-test-adapter`) をインストールする。
3. CMake Tools 拡張 (`ms-vscode.cmake-tools`) をインストールする。
4. コマンドパレットで `Python: Discover Tests` を実行する。
5. コマンドパレットで `CMake: Configure` → `CMake: Build` を実行する。
6. Testing View で pytest と C++ TestMate のテストを実行する。