# GitHub Actions テスター要件定義

## 1. 背景

- ライブラリ更新時に、既存の C++ / Python 実装（クラス・関数）の互換性が崩れるリスクがある。
- ローカル実行漏れを防ぎ、Pull Request 時点で自動的に回帰確認できる仕組みが必要。

## 2. 目的

- GitHub Actions の workflow（YAML）で、C++ と Python のテストを自動実行する。
- 失敗時にマージを止められる状態にし、ライブラリ品質を担保する。
- ローカルでも同じテストを簡単に実行できる構成にする。

## 3. 対象範囲

- 対象言語:
	- C++
	- Python
- 対象テストフレームワーク:
	- C++: Google Test
	- Python: pytest
- テスト実装方針:
	- 1つのクラス/関数に対して、複数パラメータを与えるパラメータ化テストを前提とする。
	- C++ は `INSTANTIATE_TEST_SUITE_P` などを利用する。
	- Python は `@pytest.mark.parametrize` を利用する。
- 対象確認内容:
	- クラスの基本動作（生成、代入、コピー、破棄）
	- 関数の入出力・例外/エラー時動作
	- 境界値（最小値、最大値、空入力、不正入力）

## 4. 対象外

- ベンチマーク計測
- 大規模負荷試験
- E2E 相当の長時間テスト

## 5. Workflow ファイル要件

- 配置先: .github/workflows/test.yml
- workflow 名: Test Library (CPP/Python)
- トリガー:
	- pull_request
	- push（main, doxy）
	- workflow_dispatch
- 実行条件（paths）:
	- library/**
	- tester/**
	- CMakeLists.txt
	- pyproject.toml
	- requirements*.txt
	- .github/workflows/test.yml
- 同時実行制御:
	- 同一ブランチの重複実行は cancel-in-progress でキャンセルする。

## 6. ジョブ構成要件

### 6.1 共通

- 実行環境: ubuntu-latest
- 初期ステップ:
	- actions/checkout
- タイムアウト:
	- 各 job は 10 分以内
- ローカル実行との整合性:
	- CI とローカルで同一の実行エントリを使う。
	- 例: `make test-cpp`, `make test-py`, `make test` あるいは同等のスクリプト。
	- workflow 内で直接長いコマンドを記述せず、ローカルと同じスクリプトを呼び出す。

### 6.2 C++ テスト job

- 目的: Google Test による C++ ライブラリのクラス・関数テスト
- 必須ステップ:
	- g++ のバージョン表示
	- CMake 構成（Google Test を取得/利用）
	- ビルド
	- テスト実行（ctest または gtest バイナリ直接実行）
- コンパイル/ビルド要件:
	- C++17 以上
	- Google Test を利用する（FetchContent またはサブモジュール等は実装時に選択）
- パラメータ化要件:
	- 複数入力ケースをパラメータとして定義し、同一テスト本体で実行する。
	- 正常系と異常系を同一クラス/関数に対して網羅する。
- 成功条件:
	- ビルド成功
	- 実行終了コード 0

### 6.3 Python テスト job

- 目的: pytest による Python ライブラリのクラス・関数テスト
- 必須ステップ:
	- actions/setup-python（3.11）
	- python --version
	- pytest インストール
	- テスト実行
- 実行要件:
	- PYTHONPATH にリポジトリルートを含める
	- pytest でテストを実行する
	- 例: pytest -q tester/python
	- パラメータ化には `@pytest.mark.parametrize` を利用する
- 成功条件:
	- 実行終了コード 0

### 6.4 集約 job（任意）

- 目的: C++ と Python の両方成功を 1 つのステータスで確認
- 依存: cpp-test, python-test
- 成功条件:
	- 依存 job がすべて成功

## 7. 失敗時の要件

- いずれかのテスト job が失敗した場合、workflow 全体を失敗とする。
- Pull Request では必須ステータスチェックとして設定し、失敗時はマージ不可にする。
- 失敗ログは GitHub Actions のログから原因追跡可能であること。

## 8. 受け入れ基準

以下を満たした場合に要件充足とする。

1. Pull Request 作成/更新時に自動で workflow が起動する。
2. C++ テストが Google Test で CI 上実行される。
3. Python テストが pytest で CI 上実行される。
4. どちらかが失敗するとステータスが failed になる。
5. 両方成功時にステータスが success になる。
6. main または doxy への push でも同等のテストが実行される。
7. 同一クラス/関数に対して複数パラメータのテストケースが定義されている。
8. ローカルで CI と同じエントリコマンドにより実行できる。

## 9. 運用ルール

- 新しいクラス/関数を追加した場合、対応するテストを tester 配下へ追加する。
- クラス/関数ごとに、少なくとも正常系・境界値・異常系のパラメータセットを追加する。
- テスト追加時は、失敗時に原因が分かるアサーションメッセージまたはログを残す。
- テスト実行時間が長くなりすぎた場合、テスト分割または対象見直しを行う。

## 10. テスト配置要件

- C++ テスト:
	- 例: tester/cpp/**
	- Google Test の命名規則に沿って `*_test.cpp` を配置する。
- Python テスト:
	- 例: tester/python/**
	- pytest の命名規則に沿って `test_*.py` を配置する。

## 11. ローカル実行要件

- 開発者は追加セットアップなしでテスト実行手順を把握できること。
- README か tester/README.md に以下を明記すること:
	- C++ テスト実行コマンド
	- Python テスト実行コマンド
	- 全体テスト実行コマンド
- 推奨エントリ:
	- `make test-cpp`
	- `make test-py`
	- `make test`
- CI は上記エントリをそのまま呼び出す構成とする。

## 12. 将来拡張

- Python 複数バージョン（3.10/3.11/3.12）での matrix 実行
- clang++ 併用によるコンパイラ差分検証
- テスト結果（JUnit XML）を Artifact として保存
