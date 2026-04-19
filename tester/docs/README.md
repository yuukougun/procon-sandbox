# tester/docs

このフォルダは、tester の仕組みを「ゼロ知識から実装できる」状態まで理解するための学習ドキュメントです。

対象読者:
- テスト未経験者
- Python / C++ のどちらかしか触ったことがない人
- VS Code Testing View や C++ TestMate の使い方が分からない人

このドキュメントを読み切るとできること:
- tester フォルダ構成の意味を説明できる
- Python 側 pytest テスターを追加できる
- C++ 側 GoogleTest + C++ TestMate のテスターを追加できる
- ローカル実行、VS Code 実行、CI 実行の違いを理解して運用できる
- 典型的なエラーを自力で切り分けできる

---

## 学習順序

1. [01_prerequisites.md](01_prerequisites.md)
2. [02_architecture.md](02_architecture.md)
3. [03_python_pytest.md](03_python_pytest.md)
4. [04_cpp_gtest_testmate.md](04_cpp_gtest_testmate.md)
5. [05_implementation_from_scratch.md](05_implementation_from_scratch.md)
6. [06_operations_ci.md](06_operations_ci.md)
7. [07_troubleshooting.md](07_troubleshooting.md)

---

## 現在の実装ファイル一覧

- Python テスト実装: `tester/python/test_array.py`
- C++ テスト実装: `tester/cpp/array_test.cpp`
- C++ ビルド設定: `tester/cpp/CMakeLists.txt`
- 一括実行: `Makefile`, `tester/testAll.py`
- VS Code 設定: `.vscode/settings.json`, `.vscode/c_cpp_properties.json`

---

## まず最初に実行してみるコマンド

プロジェクトルートで実行:

```bash
make test-py
make test-cpp
make test
python3 tester/testAll.py
```

上記が通る状態を「基準状態」として、以降はこの状態を壊さないように機能追加していきます。
