# 05. ゼロから tester を実装する手順

この章は「今ある tester を一度消しても、ここを読めば再構築できる」ことを目標に書いています。

## 0. 完成イメージ

最終的に次を満たします。

- Python テスト: pytest で実行できる
- C++ テスト: GoogleTest で実行できる
- VS Code Testing View で Python と C++ を実行できる
- CLI で `make test` 一発実行できる

---

## 1. 準備

## 1.1 必要ソフト

- Python 3.10+
- g++
- CMake 3.16+
- make
- VS Code

## 1.2 推奨拡張

- ms-python.python
- ms-vscode.cpptools
- ms-vscode.cmake-tools
- matepek.vscode-catch2-test-adapter (C++ TestMate)

---

## 2. ディレクトリを作る

```bash
mkdir -p tester/python
mkdir -p tester/cpp
mkdir -p .vscode
```

---

## 3. Python テストを作る

ファイル: `tester/python/test_array.py`

```python
from pathlib import Path
import sys

import pytest


REPO_ROOT = Path(__file__).resolve().parents[2]
if str(REPO_ROOT) not in sys.path:
    sys.path.insert(0, str(REPO_ROOT))

from library.library import Array


@pytest.mark.parametrize("size", [0, 1, 8])
def test_constructor_and_size(size: int) -> None:
    arr = Array(size)
    assert len(arr) == size
    assert arr.size() == size


@pytest.mark.parametrize(("size", "index"), [(0, 0), (1, -1), (1, 1)])
def test_invalid_index_raises_index_error(size: int, index: int) -> None:
    arr = Array(size)
    with pytest.raises(IndexError):
        _ = arr[index]
```

ポイント:

- ファイル名は `test_*.py`
- 関数名は `test_*`
- import 失敗を防ぐために `sys.path` を調整

---

## 4. C++ テストを作る

ファイル: `tester/cpp/array_test.cpp`

```cpp
#include <gtest/gtest.h>
#include "library.hpp"

class ConstructorSizeTest : public ::testing::TestWithParam<int> {};

TEST_P(ConstructorSizeTest, ConstructorSetsSize) {
    const int size = GetParam();
    Array arr(size);
    EXPECT_EQ(arr.size(), size);
}

INSTANTIATE_TEST_SUITE_P(BasicSizes, ConstructorSizeTest, ::testing::Values(0, 1, 8));
```

ポイント:

- まずは最小ケースで動作確認
- その後、アクセス・コピー・異常系へ拡張

---

## 5. CMake を作る

ファイル: `tester/cpp/CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.16)
project(array_cpp_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(array_cpp_test
    array_test.cpp
)

target_include_directories(array_cpp_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../library
)

target_link_libraries(array_cpp_test PRIVATE
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(array_cpp_test)
```

ポイント:

- `FetchContent` で gtest を自動取得
- `CMAKE_EXPORT_COMPILE_COMMANDS ON` で IntelliSense 安定化

---

## 6. Makefile を作る

ファイル: `Makefile`

```make
.PHONY: test-cpp test-py test

TEST_CPP_BUILD_DIR := tester/cpp/build

test-cpp:
	cmake -S tester/cpp -B $(TEST_CPP_BUILD_DIR)
	cmake --build $(TEST_CPP_BUILD_DIR)
	ctest --test-dir $(TEST_CPP_BUILD_DIR) --output-on-failure

test-py:
	PYTHONPATH=. pytest -q tester/python

test: test-cpp test-py
```

ポイント:

- ローカルと CI の実行入口を統一

---

## 7. VS Code 設定を作る

ファイル: `.vscode/settings.json`

```json
{
  "python.testing.pytestEnabled": true,
  "python.testing.unittestEnabled": false,
  "python.testing.pytestArgs": ["tester/python"],
  "python.testing.cwd": "${workspaceFolder}",
  "python.analysis.extraPaths": ["${workspaceFolder}"],

  "testMate.cpp.test.advancedExecutables": [
    {
      "pattern": "tester/cpp/build/**/*test*"
    }
  ],

  "cmake.sourceDirectory": "${workspaceFolder}/tester/cpp",
  "cmake.buildDirectory": "${workspaceFolder}/tester/cpp/build",
  "cmake.configureOnOpen": false,
  "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools",
  "C_Cpp.default.compileCommands": "${workspaceFolder}/tester/cpp/build/compile_commands.json"
}
```

ポイント:

- Python は pytest 探索パスを固定
- C++ は TestMate の実行ファイル探索パターンを指定

---

## 8. 初回実行

```bash
# Python
make test-py

# C++
make test-cpp

# 全体
make test
```

VS Code:

1. Python: Discover Tests
2. CMake: Configure
3. CMake: Build
4. Testing View で Python/C++ を実行

---

## 9. 新しいライブラリを追加した時の実装テンプレート

### 9.1 Python 側

- `library/new_feature.py` を作る
- `tester/python/test_new_feature.py` を作る
- 正常系/境界値/異常系を `parametrize` で定義

### 9.2 C++ 側

- `library/new_feature.hpp` を作る
- `tester/cpp/new_feature_test.cpp` を作る
- `tester/cpp/CMakeLists.txt` の `add_executable` に追加

例:

```cmake
add_executable(array_cpp_test
    array_test.cpp
    new_feature_test.cpp
)
```

---

## 10. 実装チェックリスト

- [ ] テストファイル名が規約に沿っている
- [ ] 正常系・境界値・異常系がある
- [ ] `make test-py` が通る
- [ ] `make test-cpp` が通る
- [ ] Testing View でケースが見える
- [ ] 失敗時メッセージで原因追跡できる

このチェックリストを満たせば、最低限の品質で tester を追加できています。
