cmake_minimum_required(VERSION 3.16)
project(cpp_tests LANGUAGES CXX)

# C++テスト全体で言語規格を統一する。
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# エディタや言語サーバ向けに compile_commands.json を出力する。
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# tester/cpp から見たリポジトリルートを解決する。
set(PROJECT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")

# テスト用のインクルード検索パス。
# CACHE 変数にしているため、CLI から次のように上書きできる:
#   cmake -DLIBRARY_DIRS="...;..." -S tester/cpp -B tester/cpp/build
set(
    LIBRARY_DIRS
    "${PROJECT_ROOT_DIR}"
    "${PROJECT_ROOT_DIR}/library"
    CACHE STRING "Semicolon-separated include directories for headers under test"
)

# gtest のグローバルインストールを前提にしないため、configure 時に依存を取得する。
include(FetchContent)

# ローカル/CI での再現性を上げるため、GoogleTest の版を固定する。
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)

# GoogleTest 推奨に合わせ、MSVC 環境でのランタイム設定を互換に保つ。
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# CTest 連携を有効化する。
enable_testing()

# unit 配下の C++ テストソースを再帰的に収集する。
# CONFIGURE_DEPENDS により、テストファイル追加/削除時に自動で再構成される。
file(GLOB_RECURSE TEST_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/unit/*_test.cpp")

# ソースツリー内に build ディレクトリが混在しても誤検出しないよう除外する。
list(FILTER TEST_FILES EXCLUDE REGEX "/build/")

# テストファイルが1つもない場合は設定ミスとして明示的に停止する。
if(NOT TEST_FILES)
    message(FATAL_ERROR "No C++ test files found under ${CMAKE_CURRENT_SOURCE_DIR}")
endif()

# gtest_discover_tests() を使って CTest へ自動登録する。
include(GoogleTest)

# テストソース1ファイルごとに実行ファイルを作成する。
foreach(test_source ${TEST_FILES})
    # 実行ファイル名は、拡張子を除いたソースファイル名を使う。
    get_filename_component(test_name ${test_source} NAME_WE)

    # ここではテストソースのみをコンパイルする。
    # この構成では CMake 側でライブラリ実装(.cpp)をリンクしないため、
    # 実装が必要なテストはテストソース側で直接 #include して取り込む。
    add_executable(${test_name} ${test_source})

    # テストコードと対象ライブラリのヘッダ探索パスを設定する。
    target_include_directories(${test_name} PRIVATE
        ${LIBRARY_DIRS}
    )

    # gtest_main がテスト実行用 main() を提供する。
    target_link_libraries(${test_name} PRIVATE
        GTest::gtest_main
    )

    # TEST/TEST_F/TEST_P で定義した個別ケースを CTest に登録する。
    gtest_discover_tests(${test_name})
endforeach()
