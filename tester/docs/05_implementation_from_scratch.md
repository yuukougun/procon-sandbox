# 05. ゼロから tester を実装する手順

この章は「今ある tester を一度消しても再構築できる」ことを目標にした実装手順書です。

## 1. ゴール定義

最終ゴール:

- Python テストを pytest で実行できる
- C++ テストを GoogleTest + CMake で実行できる
- VS Code Testing View で Python/C++ 両方を確認できる
- 将来、`library/` 以外に実装が増えても追従できる

---

## 2. ディレクトリ作成

プロジェクトルートで:

```bash
mkdir -p tester/python/unit/library
mkdir -p tester/cpp/unit/library
mkdir -p tester/docs
```

---

## 3. Python 側を作る

### 3.1 conftest.py

`tester/python/conftest.py` を作成:

```python
from __future__ import annotations

import os
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]


def _iter_import_roots():
    yield REPO_ROOT

    extra = os.environ.get("TESTER_EXTRA_PYTHONPATH", "").strip()
    if not extra:
        return

    for raw in extra.split(os.pathsep):
        path = raw.strip()
        if not path:
            continue
        candidate = Path(path)
        if not candidate.is_absolute():
            candidate = REPO_ROOT / candidate
        yield candidate.resolve()


for root in _iter_import_roots():
    root_str = str(root)
    if root.exists() and root_str not in sys.path:
        sys.path.insert(0, root_str)
```

### 3.2 テスト実装

`tester/python/unit/library/test_array.py` を作成:

```python
import pytest
from library.library import Array


def test_constructor_and_len() -> None:
    arr = Array(3, 5)
    assert len(arr) == 3
    assert arr[0] == 5


@pytest.mark.parametrize("index", [0, 1, 2])
def test_get_set(index: int) -> None:
    arr = Array(3, 0)
    arr[index] = index + 10
    assert arr[index] == index + 10


def test_out_of_bounds() -> None:
    arr = Array(2, 7)
    with pytest.raises(IndexError):
        _ = arr[2]
```

### 3.3 動作確認

```bash
make test-py
```

---

## 4. C++ 側を作る

### 4.1 テスト実装

`tester/cpp/unit/library/array_test.cpp` を作成:

```cpp
#include <gtest/gtest.h>
#include <tuple>
#include "library.hpp"

class ArrayValueTest : public ::testing::TestWithParam<std::tuple<int, int>> {};

TEST_P(ArrayValueTest, FilledWithInitialValue) {
    auto [size, init_value] = GetParam();
    Array<int> arr(size, init_value);
    ASSERT_EQ(arr.size(), size);
    for (int i = 0; i < size; ++i) {
        EXPECT_EQ(arr[i], init_value);
    }
}

INSTANTIATE_TEST_SUITE_P(
    Typical,
    ArrayValueTest,
    ::testing::Values(
        std::make_tuple(1, 0),
        std::make_tuple(4, 7),
        std::make_tuple(8, -2)
    )
);
```

### 4.2 CMakeLists.txt

`tester/cpp/CMakeLists.txt` を作成:

```cmake
cmake_minimum_required(VERSION 3.16)
project(cpp_tester LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

get_filename_component(PROJECT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
set(DEFAULT_INCLUDE_DIRS
    "${PROJECT_ROOT_DIR}/library"
    "${PROJECT_ROOT_DIR}/solve"
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

set(EXTRA_INCLUDE_DIRS "")
if(DEFINED ENV{TESTER_CPP_EXTRA_INCLUDE_DIRS} AND NOT "$ENV{TESTER_CPP_EXTRA_INCLUDE_DIRS}" STREQUAL "")
    file(TO_CMAKE_PATH "$ENV{TESTER_CPP_EXTRA_INCLUDE_DIRS}" EXTRA_INCLUDE_DIRS_RAW)
    separate_arguments(EXTRA_INCLUDE_DIRS_RAW)
    foreach(inc_dir IN LISTS EXTRA_INCLUDE_DIRS_RAW)
        if(IS_ABSOLUTE "${inc_dir}")
            list(APPEND EXTRA_INCLUDE_DIRS "${inc_dir}")
        else()
            list(APPEND EXTRA_INCLUDE_DIRS "${PROJECT_ROOT_DIR}/${inc_dir}")
        endif()
    endforeach()
endif()

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*_test.cpp")
list(FILTER TEST_SOURCES EXCLUDE REGEX "/build/")
if(TEST_SOURCES STREQUAL "")
    message(FATAL_ERROR "No C++ test sources found under ${CMAKE_CURRENT_SOURCE_DIR}")
endif()

add_executable(cpp_tester ${TEST_SOURCES})
target_include_directories(cpp_tester PRIVATE ${DEFAULT_INCLUDE_DIRS} ${EXTRA_INCLUDE_DIRS})
target_link_libraries(cpp_tester PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(cpp_tester)
```

### 4.3 動作確認

```bash
make test-cpp
```

---

## 5. VS Code 設定

`.vscode/settings.json` の例:

```json
{
  "python.testing.pytestEnabled": true,
  "python.testing.unittestEnabled": false,
  "python.testing.pytestArgs": [
    "tester/python"
  ],
  "cmake.sourceDirectory": "${workspaceFolder}/tester/cpp",
  "cmake.buildDirectory": "${workspaceFolder}/tester/cpp/build",
  "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools",
  "C_Cpp.default.compileCommands": "${workspaceFolder}/tester/cpp/build/compile_commands.json",
  "testMate.cpp.test.advancedExecutables": [
    {
      "pattern": "${workspaceFolder}/tester/cpp/build/**/cpp_tester*",
      "cwd": "${workspaceFolder}/tester/cpp/build"
    }
  ]
}
```

`.vscode/c_cpp_properties.json` の例:

```json
{
  "configurations": [
    {
      "name": "Linux",
      "includePath": [
        "${workspaceFolder}",
        "${workspaceFolder}/library",
        "${workspaceFolder}/tester/cpp",
        "${workspaceFolder}/tester/cpp/build/_deps/googletest-src/googletest/include"
      ],
      "compilerPath": "/usr/bin/g++",
      "cStandard": "c17",
      "cppStandard": "c++20",
      "intelliSenseMode": "linux-gcc-x64",
      "compileCommands": "${workspaceFolder}/tester/cpp/build/compile_commands.json",
      "configurationProvider": "ms-vscode.cmake-tools"
    }
  ],
  "version": 4
}
```

---

## 6. Makefile 連携

`Makefile` の代表ターゲット例:

```make
.PHONY: test-cpp test-py test

test-cpp:
	cmake -S tester/cpp -B tester/cpp/build
	cmake --build tester/cpp/build
	ctest --test-dir tester/cpp/build --output-on-failure

test-py:
	python3 -m pytest tester/python -q

test: test-cpp test-py
```

---

## 7. 拡張性設計のルール

将来の追加先例:

- Python: `tester/python/unit/ai/test_model_x.py`
- C++: `tester/cpp/unit/ai/model_x_test.cpp`

原則:

- テストは対象コードと同じ分類軸でフォルダを切る
- import/include はハードコードを最小化し、環境変数で補完できるようにする
- 実行入口は `make` に集約する

---

## 8. 最終チェックリスト

- `make test-py` が成功
- `make test-cpp` が成功
- `make test` が成功
- VS Code Testing View で Python/C++ が見える
- C++ TestMate で個別ケース実行できる

これを満たせば、tester を一から再構築できた状態です。
