<!-- BEGIN-NAVIGATION -->
# 目次 [home](/CONTRIBUTING.md)
- [ディレクトリ構成](/docs/directory_structure.md) - now
- [基本的なクラス設計](/docs/class_design.md)
- [コーディング時の注意](/docs/coding_guidelines.md)
- [名前の定義ルール](/docs/naming_rules.md)
- [gitの使い方](/docs/git_usage.md)
- [開発フロー](/docs/development_flow.md)
- [githubの使い方](/docs/github_usage.md)
- [Draft-Pull-Requestの方法](/docs/draft_pull_request.md)
- [issueの使い方](/docs/issue_usage.md)
- [環境構築](/docs/environment_setup.md)
- [リポジトリ設定](/docs/repo_settings.md)
- [Dockerの使い方](/docs/docker_usage.md)

## ディレクトリ構成
<!-- END-NAVIGATION -->

このリポジトリは、実装コードと開発基盤を同じ場所で運用する構成です。  
特に `C++ / Python / Rust` を横断して開発するための設定ファイル群を重視しています。

## 主要ディレクトリ

```text
REPOSITORY
├── .devcontainer/                  # Dev Containerの定義
│   ├── generic/devcontainer.json   # 通常開発環境（GPUなし）
│   ├── nvidia/devcontainer.json    # AI開発環境（GPUあり）
│   ├── Dockerfile                  # generic/nvidia共通のイメージ定義
│   ├── CreateCommand               # コンテナ起動後の初期設定
│   ├── apt-packages.txt            # apt導入パッケージ一覧
│   ├── python-requirements.txt     # Python依存一覧
│   ├── rustup-components.txt       # Rust component一覧
│   ├── rustup-targets.txt          # Rust target一覧
│   └── cargo-tools.txt             # cargo installするツール一覧
│
├── .vscode/                        # エディタ・開発体験の調整
│   ├── c_cpp_properties.json       # インクルードパス
│   └── mcp.json                    # MCP設定
│
├── .github/
│   ├── workflows/
│   │   └── docs-pages.yml          # C++/Pythonドキュメントの自動公開
│   ├── libraries.json              # ドキュメント公開対象ライブラリ定義
│   └── scripts/
│       ├── build_docs_site.py      # ドキュメント生成とデプロイの実装
│       └── detect_library_changes.py # ライブラリ変更の検出
│
├── ai_learning/
│   ├── cpp/                        # C++エンジン
│   ├── python/                     # 学習・推論・連携スクリプト
│   │   ├── system_name/            # 系統ごとにサブディレクトリ
│   │   │   └── model_name/         # モデルごとにサブディレクトリ
│   │   └── utils/                  # 共通のヘルパー関数、ロガー
│   │
│   ├── data/                       # 学習データ・モデル・ログ
│   │   ├── raw/                    # 入手したままの不変データ
│   │   └── processed/              # モデルに投入する最終的な学習用データ
│   │       └── model_name/         # モデルごとにサブディレクトリ
│   │
│   └── models/                     # 学習済みモデルの保存先
│       └── model_name/             # モデルごとにサブディレクトリ
│           ├── best_model.pth      # ベストモデルファイル
│           ├── best_model.onnx     # ベストモデルのONNX形式ファイル
│           ├── best_model.json     # ベストモデルのバージョン情報
│           ├── model_registry.jsonl # 全てのバージョンのモデル情報を記録
│           └── model_version/      # モデルバージョンごとにサブディレクトリ
│               ├── model.ckpt      # 学習済みモデルファイル
│               ├── training_log.jsonl # 学習ログ（例: 再開学習の記録）
│               └── config.json     # モデルの設定ファイル
│
├── library/                        # 共通ライブラリ（C++/Python/Rust）
│   ├── cpp/
│   │   ├── include/                # ヘッダ
│   │   │   └── library_name/       # ライブラリごとにサブディレクトリ
│   │   │       └── file_name.hpp   # ヘッダファイル
│   │   ├── src/                    # ソース
│   │   │   └── library_name/       # ライブラリごとにサブディレクトリ
│   │   │       └── file_name.cpp   # ソースファイル
│   │   └── bin/                    # ビルド成果物（例: 静的ライブラリ）
│   │       └── library_name/       # ライブラリごとにサブディレクトリ
│   │           └── file_name.o     # ソースファイル
│   ├── python/
│   │   └── package_name/           # Pythonパッケージ
│   │       ├── __init__.py         # パッケージ定義
│   │       ├── module_name.py      # モジュールファイル
│   │       └── subpackage/         # パッケージ内のサブパッケージ
│   │           ├── __init__.py     # サブパッケージ定義
│   │           └── module_name.py  # サブモジュールファイル
│   │
│   └── rust/
│       └── crate_name/             # Rustクレート
│           ├── src/                # ソース
│           │   └── lib.rs          # クレートのエントリポイント
│           └── Cargo.toml          # クレート定義
│
├── data/
│   ├── problems/                   # 問題データ
│   │   ├── problem_id/             # 問題ごとにサブディレクトリ
│   │   │   └── problem.json        # 問題ファイル
│   │   ├── benchmark/              # ベンチマーク用の問題セット
│   │   └── generate/               # 問題生成プログラム
│   └── answers/                    # 解答データ
│       └── solution_name/          # 解法ごとにサブディレクトリ
│           └── version_name/       # バージョンごとにサブディレクトリ
│               ├── problem_id/     # 問題ごとにサブディレクトリ
│               │   ├── answer.json # 解答ファイル
│               │   └── metadata.json # 問題のメタデータ
│               └── benchmark.json  # ベンチマークのスコア
│
├── solver/                         # 問題解決コード
│   └── solution_name/              # 解法ごとにサブディレクトリ
│       ├── solve.cpp               # 解法の実装
│       ├── parameters.hpp          # 解法のパラメータ設定
│       ├── utils/                  # 解法内で使うユーティリティコード
│       └── bin/                    # 解法のビルド成果物
│
├── diagrams/                       # 設計図（drawio）
│   └── ai_learning/                # AI学習関連の設計図
├── images/                         # ドキュメント用画像
│
├── docs/                           # 開発ルールと運用手順
│   ├── _navigation.md              # 目次定義（正本）
│   ├── git-hooks/pre-commit        # docs更新時の自動生成フック
│   └── script/gen-contribut        # CONTRIBUTING.md生成
├── CONTRIBUTING.md                 # docsから自動生成される統合ガイド
│
├── visualizer/                     # Desktop可視化クレート（eframe試作）
├── server/                         # 大会サーバー、他pcとの通信
├── tester/                         # 既存プログラムの動作確認
├── benchmark/                      # 回答のベンチマーク
│
├── MakeFile                        # コマンド集約
│
├── .env                            # devcontainer実行時の環境変数
├── .env.example                    # .envのテンプレート
├── .dockerignore                   # Docker build時の除外定義
├── .gitignore                      # git管理から除外するファイル定義
├── Cargo.toml                      # Rust workspaceルート
│
└── README.md                        # プロジェクト概要
```

## 構成

この章では、上のディレクトリ構成を「共同開発でどう使うか」の観点で説明します。  
特に、新規参加者が最初に迷いやすいポイント（実装場所・設定場所・運用場所）を明確化します。

> 補足: この構成はテンプレートとしての推奨形を含みます。  
> 実際の運用リポジトリで名称が異なる場合は、役割を優先して読み替えてください。

## フォルダ詳細

> ### `docs-pages.yml`ファイル
> libraryフォルダやpythonライブラリで定義したクラスや関数を **`doxygen`** / **`sphinx`** でドキュメンテーション化し、 **`github pages`** に自動デプロイする。<br>
>
> github pagesのリンクは[こちら](https://google.com)

> ### `library`フォルダ
> - ドキュメンテーションコメントを書く。**`（処理の説明ではなく、使い方の説明）`**<br>
> - **`umbrella header`** を作成し、一括includeできるようにする。<br>
> - #pragma onceを用いて **`インクルードガード`** する。
> - 大幅な変更は過去のコードにバグを生む可能性があるため、ファイルにバージョンをつける。<br>
>
>---
> #### ライブラリ作成の注意
> - 名前は長くていいから絶対に **`今後被らないような名前`** にする。（名前の定義ルールは[こちら](#名前の定義ルール)）<br>
> - 役割ごとに **`namespace`** を分割し、一つのファイルに多くのクラスや関数を定義しない。<br>
>
> クラスの基本的な設計は[こちら](#基本的なクラス設計)

> ### `solver`フォルダ
> 似た解法のsolve.cppを作るときは同じutil.hppをincludeして **`共通の処理`** を一つのファイルで管理<br>
> 処理ごとに関数に分け、関数の役割を分散する。<br>
> パラメータはsolveファイルに書かず、hppに分け、定数値として定義する。

> ### `tester`フォルダ
> コンパイルできるか、関数やクラスが適切な値を返すかを確認する。<br>
> push時などにci.ymlを実行し、動作確認を行う。<br>
> testerコードはtesterフォルダ内に置き、ローカル環境でも動かせるようにする。

> ### `benchmark`フォルダ
> 解答のスコアを測定する。

> ### `Makefile`ファイル
> ファイルのコンパイルや実行などターミナルで実行するコマンド処理は全てこれで行う。（gitコマンドは除く）<br>
> オブジェクトファイルの作成やリンクもこれで行う。
