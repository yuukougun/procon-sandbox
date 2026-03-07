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

```
REPOSITORY
├── .github
│   └── workflows
|       ├── test.yml    # pull request時にtesterを実行し、コメント
|       ├── linter.yml  # pull request時にリンターを実行し、コメント
│       └── doxygen.yml # ライブラリの設計仕様をpagesにデプロイ
├── .vscode
│   ├── settings.json   # 設定ファイル
│   ├── extensions.json # 推奨拡張機能のリスト
│   └── c_cpp_properties.json   # コンパイラ設定ファイル
│
├── library             # すべてのライブラリのフォルダ 
│   └── LIBRARY_DESIGN.md       # ライブラリの設計仕様 
├── solve               # 問題のsolveファイルのフォルダ
│   ├── solve.cpp       # solveファイル
│   ├── util.hpp        # soleファイルの補助ライブラリ
│   └── parameters.hpp  # パラメータファイル
├── input               # inputデータのフォルダ
├── output              # outputデータのフォルダ
├── server              # サーバーやpc間の連携を行うフォルダ
├── visualizer          # 回答データや分析結果の可視化
│   └── analysis        # 回答データを分析したデータのフォルダ
├── tester              # 関数やクラスの実行テストフォルダ
│   └── testAll.py      # すべてのテストを行うファイル
├── diagrams            # 設計図のフォルダ
│   └── design.drawio   # ライブラリやワークフローの設計図
├── images              # Readmeに記す画像のフォルダ
│
│
├── .gitignore          # リポジトリに含めないファイルの設定
├── Doxyfile            # doxygenの設定ファイル
├── Makefile            # リンカコンパイルの設定ファイル
├── run.sh              # すべてのコマンド操作を行うファイル
├── COMMAND.md          # run.shなどのコマンドの解説
├── CONTRIBUTING.md     # 共同開発者にリポジトリのルールの解説
└── README.md           # リポジトリの説明（使い方など）
```

> ### `doxygen.yml`ファイル
> libraryフォルダで定義したクラスや関数を **`doxygen`** でドキュメンテーション化し、 **`github pages`** に自動デプロイする。<br>
>
> github pagesのリンクは[こちら](https://google.com)

> ### `library`フォルダ
> - ドキュメンテーションコメントを書く。**`（処理の説明ではなく、使い方の説明）`**<br>
> - solveで使用するc++のクラスや関数をまとめる。<br>
> - 関数には **`inline`** をつけてリンクできるようにする<br>
> - serverやvisualizerのpythonライブラリを作る場合は **`library / pythonフォルダ`** 内に置く。<br>
> - ライブラリを作成する前にLIBRARY_DESIN.mdに設計仕様をまとめ、計画的にクラスを作成する。 **`（他の人と相談しながらがいい）`**<br>
> - **`umbrella header`** を作成し、一括includeできるようにする。<br>
> - #pragma onceを用いて **`インクルードガード`** する。
>
>---
> #### ライブラリ作成の注意
> - 名前は長くていいから絶対に **`今後被らないような名前`** にする。（名前の定義ルールは[こちら](#名前の定義ルール)）<br>
> - ライブラリファイルは **`基本変更しない`** ような設計にする。（パラメータ値はsolveフォルダ内に書きincludeするか、引数で渡す）<br>
> - 例外処理か、エラー出力を行う。エラー出力では内部データを出力し、**`デバックしやすくする`**。<br>
> - 右辺値や左辺値、constを意識して **`無駄なコピー`** が発生しないようなクラスにする。<br>
> - 役割ごとに **`namespace`** を分割し、一つのファイルに多くのクラスや関数を定義しない。<br>
> - クラス内に構造体が必要になったら **`クラス内に書く`** 。<br>
> - クラス内のメンバ変数はpublicにせず、getterやsetterでやり取りする。<br>
> - 同じ関数でも引数を変えてオーバーライドし、様々な状況で使いやすくする。<br>
> - templateを用いて柔軟なクラスにする。（競技のデータ構造による）<br>
> - conceptやrequiresを用いて **`templateの型を制限する`**。<br>
> - コンストラクタには基本 **`explicit`** をつけ、暗黙の型変換を防止する。<br>
> - コンパイル時に計算できる可能性のある関数は基本 **`contexpr`** をつける。<br>
> - 右辺値参照する場合 **`noexcept`** をつける。（例外を投げる可能性があるなら書かない）
>
> クラスの基本的な設計は[こちら](#基本的なクラス設計)

> ### `solve`フォルダ
> 大まかな解法ごとにフォルダをネストさせる。<br>
> 処理ごとにutil.hppでファイルを分け、includeする。（ライブラリフォルダではなくsolveフォルダに入れる）<br>
> 似た解法のsolve.cppを作るときは同じutil.hppをincludeして **`共通の処理`** を一つのファイルで管理<br>
> 処理ごとに関数に分け、関数の役割を分散する。<br>
> パラメータはsolveファイルに書かず、hppに分け、定数値として定義する。


> ### `server`フォルダ
> 大会サーバーやpc間の通信処理はここで行う。<br>
> guiのボタンで簡単に操作できるようにする。

> ### `visualizer`フォルダ
> 回答データや分析データをguiで可視化する。<br>
> 結果だけの表示ではなく **`途中過程`** でどういった操作が行われたかを可視化する。<br>
> run.shと被るが、solveを実行しそれぞれの入力例を選択できるようにすると便利

> ### `tester`フォルダ
> コンパイルできるか、関数やクラスが適切な値を返すか、短時間で確認できるテストを試す。<br>
> push時などにtest.ymlを実行し、動作確認を行う。<br>
> テスト方法は要検討

> ### `analysis`フォルダ
> 回答データを分析し、推移や解法ごとの分析データを保存する<br>
> 状態変化をgifとして保存するのもあり

> ### `run.sh`ファイル
> ファイルのコンパイルや実行などターミナルで実行するコマンド処理は全てこれで行う。（gitコマンドは除く）<br>
> makefileでオブジェクトファイルの作成やリンクもこれで行う。
