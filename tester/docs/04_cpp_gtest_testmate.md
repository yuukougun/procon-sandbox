# 04. C++ テスター実装ガイド (GoogleTest + C++ TestMate)

この章では、C++ 側の tester を C++ TestMate 中心で運用するための実装と設定を説明します。

## 1. 現在のテスト対象

対象実装:

- `library/library.hpp`
- クラス `Array`

検証している主な観点:

- コンストラクタとサイズ整合
- 要素アクセス整合
- コピーの独立性
- allocate / clear の動作
- move 代入
- 範囲外アクセスの assert (Debug 時)

---

## 2. CMakeLists.txt の役割

ファイル:

- `tester/cpp/CMakeLists.txt`

役割:

1. C++17 でビルドする
2. GoogleTest を取得する (FetchContent)
3. テスト実行ファイルを生成する
4. include path を与える
5. (必要に応じて) CTest へ登録する

補足:

- C++ TestMate を使う場合でも、テスト実行ファイル生成の仕組みは必要です。
- 現構成では CMake がその役割を担います。

---

## 3. GoogleTest の基本

## 3.1 通常テスト

```cpp
TEST(SuiteName, CaseName) {
    EXPECT_EQ(1 + 1, 2);
}
```

## 3.2 パラメータ化テスト

現在実装は `TEST_P` + `INSTANTIATE_TEST_SUITE_P` を使っています。

```cpp
class ConstructorSizeTest : public ::testing::TestWithParam<int> {};

TEST_P(ConstructorSizeTest, ConstructorSetsSize) {
    const int size = GetParam();
    Array arr(size);
    EXPECT_EQ(arr.size(), size);
}

INSTANTIATE_TEST_SUITE_P(BasicSizes, ConstructorSizeTest, ::testing::Values(0, 1, 8));
```

利点:

- 同じロジックを多数の入力で再利用できる
- 失敗時にどの入力が原因か追跡しやすい

## 3.3 Death Test

```cpp
EXPECT_DEATH({ arr.at(2); }, ".*");
```

用途:

- `assert` でプロセス終了する挙動を検証

注意:

- Release ビルドでは `NDEBUG` により assert が無効な場合がある
- そのため現実装は `#ifndef NDEBUG` で囲んでいる

---

## 4. C++ TestMate の設定

`.vscode/settings.json` の実設定:

```json
"testMate.cpp.test.advancedExecutables": [
  {
    "pattern": "tester/cpp/build/**/*test*"
  }
]
```

意味:

- build 配下のテスト実行ファイル候補を探索
- gtest バイナリを見つけたらケース一覧化

運用手順:

1. `make test-cpp` または CMake Build を実行
2. バイナリ生成後に Testing View を開く
3. C++ TestMate セクションでケース実行

---

## 5. include エラー (1696) 対策

典型例:

- `#include <gtest/gtest.h>` が赤波線

主な原因:

- IntelliSense が gtest のヘッダ場所を知らない
- compile_commands.json が未生成

対処方針:

1. `CMAKE_EXPORT_COMPILE_COMMANDS ON` を CMake に設定
2. CMake configure を実行
3. `.vscode/c_cpp_properties.json` で compileCommands / includePath を設定
4. 必要なら IntelliSense DB をリセット

---

## 6. 新しい C++ テストを追加する手順

1. `tester/cpp/<name>_test.cpp` を作る
2. 対象ヘッダを include
3. `TEST` / `TEST_P` を実装
4. `tester/cpp/CMakeLists.txt` の `add_executable` に追加
5. `make test-cpp` で確認
6. Testing View でケース実行

推奨観点:

- 正常値
- 境界値
- 不正入力
- コピー / ムーブの所有権
- 例外安全 (必要に応じて)

---

## 7. 実行コマンド

```bash
# configure + build + ctest
make test-cpp

# 手動 configure
cmake -S tester/cpp -B tester/cpp/build

# 手動 build
cmake --build tester/cpp/build

# 手動 test
ctest --test-dir tester/cpp/build --output-on-failure
```

---

## 8. この章の完了条件

- gtest の通常テストとパラメータ化テストを追加できる
- CMake と TestMate の連携理由を説明できる
- include エラーを自力で復旧できる
