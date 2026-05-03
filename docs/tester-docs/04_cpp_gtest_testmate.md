<!-- BEGIN-NAVIGATION -->
# 目次 [home](/docs/tester-docs/README.md)
- [1. 前提知識](/docs/tester-docs/01_prerequisites.md)
- [2. アーキテクチャ](/docs/tester-docs/02_architecture.md)
- [3. Python pytest](/docs/tester-docs/03_python_pytest.md)
- [4. C++ GoogleTest + C++ TestMate](/docs/tester-docs/04_cpp_gtest_testmate.md) - now
- [5. ゼロからの実装](/docs/tester-docs/05_implementation_from_scratch.md)
- [6. 運用とCI](/docs/tester-docs/06_operations_ci.md)
- [7. トラブルシューティング](/docs/tester-docs/07_troubleshooting.md)

## 4. C++ GoogleTest + C++ TestMate
<!-- END-NAVIGATION -->

# 04. C++ テスト (GoogleTest + C++ TestMate)

この章では、C++ 側テスターを「CMake でビルドし、C++ TestMate で運用する」流れを解説します。

## 1. 役割分担を先に理解する

- GoogleTest: テストコードを記述するフレームワーク
- CMake: テストバイナリをビルドする仕組み
- CTest: CMake で登録したテストを CLI 実行する仕組み
- C++ TestMate: VS Code でテスト一覧を表示・実行する UI

重要:

- TestMate 単体ではビルドしない
- 先に CMake でバイナリを作る必要がある

---

## 2. 対象ファイル

- 実装対象（例）: `library/library.hpp`
- テスト実装（例）: `tester/cpp/unit/library/array_test.cpp`
- ビルド設定: `tester/cpp/CMakeLists.txt`

---

## 3. テストコードの基本形

```cpp
#include <gtest/gtest.h>
#include "library.hpp"

TEST(ArrayBasic, Construction) {
    Array<int> arr(3, 7);
    EXPECT_EQ(arr.size(), 3);
    EXPECT_EQ(arr[0], 7);
}
```

ポイント:

- 1 テスト 1 意図
- ケース名で目的が分かる命名にする

---

## 4. パラメータ化テスト

複数条件を同じロジックで検証する場合は `TEST_P` を使う。

```cpp
class ArrayValueTest : public ::testing::TestWithParam<std::tuple<int, int>> {};

TEST_P(ArrayValueTest, FillValue) {
    auto [size, value] = GetParam();
    Array<int> arr(size, value);
    ASSERT_EQ(arr.size(), size);
    for (int i = 0; i < size; ++i) {
        EXPECT_EQ(arr[i], value);
    }
}

INSTANTIATE_TEST_SUITE_P(
    BasicCases,
    ArrayValueTest,
    ::testing::Values(
        std::make_tuple(1, 0),
        std::make_tuple(4, -3),
        std::make_tuple(8, 9)
    )
);
```

---

## 5. CMake の重要点

現在の `tester/cpp/CMakeLists.txt` は次を実施します。

- gtest を FetchContent で取得
- `*_test.cpp` を再帰探索
- `/build/` 配下は除外
- `TESTER_CPP_EXTRA_INCLUDE_DIRS` で include 追加
- `gtest_discover_tests` で CTest 登録

これにより、将来 `tester/cpp/unit/ai/xxx_test.cpp` のような追加にもそのまま対応できます。

---

## 6. 実行方法

### 6.1 CLI

```bash
make test-cpp
```

内部処理:

1. `cmake -S tester/cpp -B tester/cpp/build`
2. `cmake --build tester/cpp/build`
3. `ctest --test-dir tester/cpp/build --output-on-failure`

### 6.2 VS Code + C++ TestMate

1. 事前に `make test-cpp` でバイナリ生成
2. VS Code Testing View を開く
3. C++ テストツリーを表示
4. ケース単位で実行

---

## 7. 新しい C++ テスト追加手順

例: `ai/cpp/` の実装をテストしたい場合

1. 配置先を作る

```text
tester/cpp/unit/ai/
```

2. `*_test.cpp` を作る

3. include が足りなければ環境変数で追加

```bash
export TESTER_CPP_EXTRA_INCLUDE_DIRS="ai/cpp/include:ai/cpp/src"
```

4. `make test-cpp` 実行

5. TestMate で表示確認

---

## 8. IntelliSense エラー 1696 対応

症状:

- `#include <gtest/gtest.h>` に赤波線

主因:

- cpptools が gtest include パスを解決できていない

対処:

1. `make test-cpp` を一度実行し build 生成
2. `.vscode/c_cpp_properties.json` の `compileCommands` を確認
3. `.vscode/settings.json` の CMake 設定を確認
4. VS Code の C/C++ IntelliSense を再読み込み

---

## 9. この章の完了条件

- 新規 C++ テストを `tester/cpp/.../*_test.cpp` へ追加できる
- `make test-cpp` で実行できる
- TestMate でケース実行できる
- gtest include エラーの基本原因を説明できる
