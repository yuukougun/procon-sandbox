<!-- BEGIN-NAVIGATION -->
# 目次 [home](/docs/tester-docs/README.md)
- [1. 前提知識](/docs/tester-docs/01_prerequisites.md)
- [2. アーキテクチャ](/docs/tester-docs/02_architecture.md) - now
- [3. Python pytest](/docs/tester-docs/03_python_pytest.md)
- [4. C++ GoogleTest + C++ TestMate](/docs/tester-docs/04_cpp_gtest_testmate.md)
- [5. ゼロからの実装](/docs/tester-docs/05_implementation_from_scratch.md)
- [6. 運用とCI](/docs/tester-docs/06_operations_ci.md)
- [7. トラブルシューティング](/docs/tester-docs/07_troubleshooting.md)

## 2. アーキテクチャ
<!-- END-NAVIGATION -->

# 02. tester の構成と仕組み

この章では、現在の tester の構成を「なぜその設計にしているか」まで含めて説明します。

## 1. ディレクトリ構成

```text
tester/
├── README.md
├── testAll.py
├── python/
│   ├── conftest.py
│   └── unit/
│       └── library/
│           └── test_array.py
├── cpp/
│   ├── CMakeLists.txt
│   ├── unit/
│   │   └── library/
│   │       └── array_test.cpp
│   └── build/
└── docs/
    ├── README.md
    ├── 01_prerequisites.md
    ├── 02_architecture.md
    ├── 03_python_pytest.md
    ├── 04_cpp_gtest_testmate.md
    ├── 05_implementation_from_scratch.md
    ├── 06_operations_ci.md
    └── 07_troubleshooting.md
```

## 2. 各ファイルの役割

### 2.1 tester/python/conftest.py

- pytest 起動時に共通の import パス設定を行う。
- リポジトリルートを自動追加。
- `TESTER_EXTRA_PYTHONPATH` で追加フォルダを動的に拡張可能。

### 2.2 tester/python/unit/library/test_array.py

- Python 実装 (`library/library.py`) を検証する pytest テスト。
- 例外確認、境界値確認、コピーの独立性確認などを担当。

### 2.3 tester/cpp/unit/library/array_test.cpp

- C++ 実装 (`library/library.hpp`) を検証する GoogleTest テスト。
- `TEST_P` と `INSTANTIATE_TEST_SUITE_P` でパラメータ化。
- 必要に応じて `EXPECT_DEATH` で assert の動作を確認。

### 2.4 tester/cpp/CMakeLists.txt

- gtest の取得、ビルド設定、テストバイナリ生成を担当。
- `*_test.cpp` を再帰的に探索し、階層化されたテストも自動収集する。
- `TESTER_CPP_EXTRA_INCLUDE_DIRS` で include 対象を追加できる。
- TestMate はこのバイナリを読んでテスト一覧表示する。

### 2.5 Makefile

- 開発者が統一コマンドでテストできるようにする入口。
- CI でも同じ入口を使うことで「ローカルでは通るのに CI は失敗」を減らす。

### 2.6 tester/testAll.py

- Python から `make test-cpp` と `make test-py` を順番に呼ぶラッパー。
- 実装や自動化ツールで Python 側から統合実行したいときに使う。

### 2.7 .vscode/settings.json

- Python テストの検出条件を指定。
- C++ TestMate の実行バイナリ探索パターンを指定。

---

## 3. 実行フロー

### 3.1 ローカル CLI 実行フロー

1. `make test-cpp`
2. CMake が `tester/cpp/build` を更新
3. gtest バイナリ生成
4. `ctest` が C++ テスト実行
5. `make test-py`
6. pytest が Python テスト実行

### 3.2 VS Code Testing View 実行フロー

Python:

1. Python 拡張が `tester/python/**/test_*.py` を検出
2. Testing View にケース表示
3. ケース単位で実行

C++:

1. CMake ビルドで gtest バイナリ生成
2. C++ TestMate が設定パターンに一致するバイナリを検出
3. テストケース一覧を表示
4. ケース単位またはスイート単位で実行

### 3.3 CI 実行フロー

1. GitHub Actions が起動
2. `make test-cpp` 実行
3. `make test-py` 実行
4. どちらか失敗で workflow 失敗

---

## 4. 依存関係の考え方

依存の方向は次のようになります。

- `library/*` は「テストされる対象」の一例
- `ai/*` や `python/*` など他フォルダでも同じ仕組みで対象化可能
- `tester/*` は「検証コード」
- `Makefile` は「実行導線」
- `.vscode/*` は「開発体験の設定」

この分離を守ると、将来ライブラリが増えても tester を拡張しやすくなります。

---

## 5. 旧テスターとの関係

`tester/tester.py` や `tester/tester.cpp` は手動実行型の旧スタイルです。

現在の推奨運用は以下:

- Python: pytest 形式を `tester/python/<layer>/<category>/` 配下へ追加
- C++: GoogleTest 形式を `tester/cpp/<layer>/<category>/` 配下へ追加
- 実行は `make` と Testing View を中心に統一

---

## 6. この章の完了条件

以下を説明できれば次章へ進んで良いです。

- tester 内の各ファイルの責務
- CLI / Testing View / CI の実行経路
- TestMate と CMake の役割分担
