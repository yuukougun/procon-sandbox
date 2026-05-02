<!-- このファイルは自動生成されるものです。書き換えても内容は破棄されます。-->
# proconでのリポジトリの使い方

# 目次 [home](/CONTRIBUTING.md)
- [ディレクトリ構成](/docs/directory_structure.md)
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
    - [testerのドキュメント](/docs/tester-docs/README.md)
    - [copilotのドキュメント](/docs/copilot-docs/github-copilot-cli-guide.md)

<a id="ディレクトリ構成"></a>
<details>
<summary><span style="font-size:1.5em; font-weight:bold;">ディレクトリ構成</span></summary>

---

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
</details>

---

<a id="基本的なクラス設計"></a>
<details>
<summary><span style="font-size:1.5em; font-weight:bold;">基本的なクラス設計</span></summary>

---

このドキュメントは、言語ごとに実装スタイルがぶれないための最小ルールをまとめる。

## C++クラス設計

- クラスは単一責任原則を意識し、1クラス1機能を目指す
- コピーが重い型は参照渡しを優先する
- 必要に応じて `explicit`, `noexcept`, `const` を使う

```cpp
template <typename T>
concept Addable = requires(T a, T b){
    {a + b};
}

template <Addable T>
class MyClass{
    protected:
        // メンバ変数や定数
        string name;
        vector<T> vec_data;

    public:
        //コンストラクタ
        explicit MyClass();
        explicit MyClass(const MyClass& other);
        explicit MyClass(MyClass&& other) noexcept;

        //デストラクタ
        virtual ~MyClass();

        //アクセサ
        string get_name() const;
        void set_name(string other_name);

        //イテレータ
        typename vector<T>::iterator begin();
        typename vector<T>::iterator end();
        typename vector<T>::const_iterator begin() const;
        typename vector<T>::const_iterator end() const;

        //演算子
        MyClass& operator=(const MyClass& other);
        MyClass& operator=(MyClass&& other) noexcept;
        auto operator<=>(const MyClass& other) const;
        auto operator==(const MyClass& other) const;
        vector<T>& operator[](size_t index);
        const vector<T>& operator[](size_t index) const;

        //コンバータ
        explicit operator bool() const;

        //出力
        void print(ostream& os = cout) const;

    private:
        //クラス構築
        void build(const vector<T>& data);
        void build(vector<T>&& data);

}
```

> ### 設計時の注意
> - クラスを作成する前にLIBRARY_DESIN.mdに設計仕様をまとめ、計画的にクラスを作成する。 **`（他の人と相談しながらがいい）`**<br>
> - ライブラリファイルは **`基本変更しない`** ような設計にする。（パラメータ値はsolveフォルダ内に書きincludeするか、引数で渡す）<br>
> - 例外処理か、エラー出力を行う。エラー出力では内部データを出力し、**`デバックしやすくする`**。<br>
> - 右辺値や左辺値、constを意識して **`無駄なコピー`** が発生しないようなクラスにする。<br>
> - クラス内に構造体が必要になったら **`クラス内に書く`** 。<br>
> - conceptやrequiresを用いて **`templateの型を制限する`**。<br>
> - コンパイル時に計算できる可能性のある関数は基本 **`contexpr`** をつける。
</details>

---

<a id="コーディング時の注意"></a>
<details>
<summary><span style="font-size:1.5em; font-weight:bold;">コーディング時の注意</span></summary>

---

このプロジェクトは `C++ / Python / Rust` の混在開発を前提としているため、
「言語ごとの書き方」より「全体で読みやすいこと」を優先する。

## 共通ルール

- 変数名・関数名・型名は日本語を使わない
- 略語は最小限にし、意味が分かる名前を使う
- 1コミット1目的を意識し、差分を小さく保つ
- 処理を追加するときはブランチを切る。
- 実装変更時は関連ドキュメントも更新する

- 一時的な変数は以下のように書き、延命を防ぐ。
```cpp
int main(){
    int n = 1;
    {
        int tmp = n * 2;
        cout << tmp << endl;
    }
}
```

- {}内の処理が長くなる場合は以下のように最後にコメントを書く。
```cpp
namespace test{
    // ...処理...
}//namespace test
```

## コメント方針

- 「何をしているか」ではなく「なぜ必要か」を優先して書く
- 複雑な分岐や運用上の制約には短い説明を残す
</details>

---

<a id="名前の定義ルール"></a>
<details>
<summary><span style="font-size:1.5em; font-weight:bold;">名前の定義ルール</span></summary>

---

命名は「検索しやすさ」と「衝突しにくさ」を重視する。

> ### 一般的な命名規則として５つに分類される。
> 1. パスカルケース（MyName）
> 2. キャメルケース（myName）
> 3. スクリーミングケース（MY_NAME）
> 4. スネークケース（my_name）
> 5. ケバブケース（my-name）

> ### 種類別の命名規則
> | 種類 | 命名規則 |
> | --- | --- |
> | ファイル、フォルダ | スネークケース（my_name） |
> | 型 | パスカルケース（MyName） |
> | クラス | パスカルケース（MyName） |
> | 関数、メソッド | スネークケース（my_name） |
> | 変数 | スネークケース（my_name） |
> | 定数 | スクリーミングケース（MY_NAME） |
> | 真偽値（bool）| is_my_name |

> ### ブランチの命名規則
> ブランチ名には'/'をつけることができる。<br>
> ブランチ名の前にそのブランチの種類を書く（例：bugfix/library）<br>
> 大きなプロジェクトのブランチはmainを増設する。（例：visualizer/main）<br>
> 例：visualizerでbuttonのバグを治すブランチは **`visualizer/bugfix/button`**
> 
> | 種類 | 命名規則 |
> | --- | --- |
> | 機能追加 | feature |
> | バグ修正 | bugfix |
> | コード整理 | refactor |
> | テストコード | test |
> | ドキュメント整理 | docs |
>
> バグの修正でissueを修正する場合は **`bugfix/issue-(issueの番号)`** にする。
</details>

---

<a id="gitの使い方"></a>
<details>
<summary><span style="font-size:1.5em; font-weight:bold;">gitの使い方</span></summary>

---

## 基本方針

- `main` への直接コミットは避け、原則PR経由でマージする
- 1ブランチ1目的で作業する
- コミットメッセージは日本語で、変更意図が分かるように書く
- ブランチはdraft PRを作成してから切る
- コマンド実行時に途中で衝突したら実行が止まるため、--continueで再開
- コマンドを途中で中断するときは、--abort

> ### gitコマンド一覧
> 共同開発で使うものだけを一覧にしたため、他のコマンドは使わないと思う。<br>
> より多くのgitコマンドは[こちら](https://zenn.dev/zmb/articles/054ba4189244a5) 
> git用語は[こちら](https://qiita.com/shinshingodmt/items/637cf9e5c6660509c460)
>
> | コマンド | 効果 |
> | --- | --- |
> | git status | 現在の状態を表示 |
> |  |  |
> | git add (ファイル名) | (ファイル名)をステージングする |
> | git add . | すべてのファイルをステージングする |
> | git commit -m (コミットメッセージ) | ステージングファイルをコミットする |
> | git commit --amend -m (コミットメッセージ) | 直前のコミットメッセージを変更（ステージング状態は空にしておく） |
> | git commit --allow-empty -m "空のコミット" | 空のコミットを作成する |
> | git merge (ブランチ名) | (ブランチ名)を現在のブランチにマージ（mainにはpull requestする）|
> | git merge --continue | mergeの再開 |
> |  |  |
> | git push | 現在のブランチのローカルの変更内容をリモートに送信 |
> | git push (ブランチ名) | (ブランチ名)のローカルの変更内容をリモートに送信 |
> | git push -u origin (ブランチ名) | リモートに(ブランチ名)を追加してpushする |
> | git push origin -d (ブランチ名) | リモートの(ブランチ名)を削除 |
> | git pull | 現在のブランチのリモートの変更内容をローカルに取り込む |
> | git pull origin (ブランチ名) | (ブランチ名)のリモートの変更内容をローカルに取り込む |
> | git pull --rebase | 自分のcommitを他人のcommitの後に変える |
> | git fetch --all | ローカルを最新の状態に更新（ローカルリポジトリは変更しない）|
> |  |  |
> | git branch | ブランチの一覧を表示 |
> | git branch -m (現在のブランチ名) (新規ブランチ名) | ブランチ名の変更 |
> | git branch -d (ブランチ名) | (ブランチ名)を削除 |
> | git branch -D (ブランチ名) | マージしてない(ブランチ名)を削除（超危険）|
> | git checkout (ブランチ名) | (ブランチ名)に切り替え |
> | git checkout (コミット値) | (コミット値)に切り替え |
> | git checkout -b (ブランチ名) | (ブランチ名)のブランチを作成 |
> | git checkout (ブランチ名) -- (ファイル名) | (ブランチ名)から(ファイル名)をコピーしadd |
> |  |  |
> | git stash | 編集した内容を退避 |
> | git stash list | stashの一覧を表示（stashの番号はこれで確認）|
> | git stash show stash@{(番号)} | (番号)番目のstashを詳細表示 |
> | git stash save (コメント) | stashに(コメント)をつけてstashする |
> | git stash pop | 編集した内容を呼び出す |
> | git stash pop stash@{(番号)} | (番号)番目のstashでpopする |
> | git stash drop stash@{(番号)} | (番号)番目のstashを削除（超危険） |
> |  |  |
> | git rebase (ブランチ名) | (ブランチ名)にrebaseする |
> | git rebase --continue | rebaseの再開 |
> | git cherry-pick (コミット値) | (コミット値)を取り込む |
> | git cherry-pick (コミット値1)..(コミット値2) | (コミット値1)の次から(コミット値2)を順にcherry-pick |
> | git cherry-pick --continue | cherry-pickの再開 |
> |  |  |
> | git revert HEAD | 直前のコミットを打ち消すコミットを追加 |
> | git revert (コミット値) | (コミット値)のコミットを打ち消すコミットを追加 |
> | git revert -m 1 (マージのコミット値) | マージを打ち消すコミットを追加 |
> | git restore (ファイル名) | (ファイル名)で編集した内容を破棄（超危険） |
> | git restore . | すべてのファイルで編集した内容を破棄（超危険） |
> |  |  |
> | git reflog | git操作の状態履歴を表示（履歴の番号はこれで確認） |
> | git reset --soft ORIGIN_HEAD | マージ、リベース、リセットの処理をもとに戻す（危険） |
> | git reset --soft HEAD^ | 直近１個のコミットをステージング状態に戻す。（危険）|
> | git reset --soft HEAD~(個数) | 直近(個数)個のコミットをステージング状態に戻す（危険） |
> | git reset --soft (コミット値) | (コミット値)の状態に戻す（危険） |
> | git reset --soft HEAD@{(番号)} | リポジトリを(番号)番目の状態に戻す（危険） |
> |  |  |
> | git tag | タグの一覧を表示 |
> | git tag -a (タグ名) -m "(タグのコメント)" | 直近のコミットにタグを付ける |
> | git tag -a (タグ名) -m "(タグのコメント)" (コミット値) | (コミット値)にタグを付ける |
> | git tag -d (タグ名) | タグを削除 |
</details>

---

<a id="開発フロー"></a>
<details>
<summary><span style="font-size:1.5em; font-weight:bold;">開発フロー</span></summary>

---

git用語は[こちら](https://qiita.com/shinshingodmt/items/637cf9e5c6660509c460)

- 進捗共有はDraft Pull Requestを起点にする
- 変更は小さく、頻繁にpushする
- 基本的に開発時のメモは[discussionやwiki](#githubの使い方)を使う
- 一旦の数値等の一時的なメモはmemoファイルに書く

## 目次
- [図の説明](#図の説明)
- [リモートとローカルの違い](#リモートとローカルの違い)
- [stashの使い方](#stashの使い方)
- [同じブランチで共同者がコミットしていた時の対処（rebase）](#同じブランチで共同者がコミットしていた時の対処（rebase）)
- [バグや改善を発見した場合の対処](#バグや改善を発見した場合の対処)
- [バグ修正など一部のコミットだけ取り込む（cherry-pick）](#バグ修正など一部のコミットだけ取り込む（cherry-pick）)
- [pull requestする](#pull-requestする)

> ### 図の説明
> ![画像](/images/contributing/explanation.png)
> 
> - リモートのコミットは枠線
> - ローカルのコミットは塗りつぶし
> - 塗り潰し、枠線の色でブランチの違い
> - ステージング、ステージング前の一時的な変更は灰色
> - コミット値はコミット内の数値
> - 空のコミットはコミット内に空

> ### リモートとローカルの違い
> - ローカルで編集しただけでは、リモートには反映されないため、pushする。<br>
> - 共同者も同時に編集するため、リモートは常に更新される。<br>
> - ローカルの編集とリモートの編集が衝突しないよう、リモートに **`pushする前に必ずfetch`** を行う。<br>
> - なにかリモートに変更を加えるときは、最新のリモートの状態を把握する。<br>
> - ローカルで大量の編集を貯めず、頻繁にpushする。

> ### stashの使い方
> stashコマンドの一覧は[こちら](#gitの使い方)<br>
> 自分のブランチで修正している時に別のブランチを編集したいとき、
> stashコマンドを用いることでステージング、ステージング前の変更がすべて保存される。<br>
> 保存した変更を別のブランチで展開することもできるが、なるべく使用を避ける。
>
> 下図のようにstashと一時的な変更が衝突した時、ステージング状態にしてから展開する。
> 
> ![画像](/images/contributing/exp_stash.png)
> 
> 1. 現在の変更をステージングする。
> 2. **`git stash pop`** で展開する。
> 3. 衝突したら処理して **`git stash list`** で確認する。
> 4. stashが消えてなかったら **`git stash drop stash@{(番号)}`** で消す。
> 
> 上の手順を行うことで一時的な変更を一つにまとめれる。

> ### 同じブランチで共同者がコミットしていた時の対処（rebase）
> 下図のようにローカルで編集した内容をpushする際に、リモートで共同者がコミットしていたとき、そのままpushすることはできない。<br>
> このとき、rebaseを行い自分の変更をリモートブランチの後ろにコミットし直す。
> 
> ![画像](/images/contributing/exp_rebase.png)
> 
> 1. ローカルで行った編集をコミットする。（stashで後から復元すると衝突処理がめんどい）
> 2. **`git rebase origin/(ブランチ名)`** でリモートブランチにリベースする。
> 3. 途中で衝突が発生した場合、修正を行いステージング状態にする。**`（絶対にコミットしない）`**
> 4. 衝突処理が完了したら **`git rebase --continue`** でリベースを再開する。
>
> 上の手順を行うことで下図の状態となる。（一時的な編集はコミットしない場合）<br>
> 複数人で同じブランチを直接編集することは避ける。
>
> ![画像](/images/contributing/exp_rebase_after.png)

> ### バグや改善を発見した場合の対処
> 自分のブランチで作業している時にバグや修正を発見した場合。<br>
> 自分のブランチのみで発生したものはブランチ内で解決するべきであるが、
> mainブランチなど、すでに共同者が利用している箇所で発生したものは **`ブランチを変えて編集`** する。<br>
> 発見した瞬間に忘れないよう **`issueとして登録`** し、共同者に共有する。（自分では変更できない場合は共同者に任せる）<br>
> 1コミットですむ数行程度の変更なら新しくブランチを分けずmainにそのままコミットする。<br>
> 少し手間がかかる変更はbugfixやfeatureブランチとして分ける。
>
> 下図のfeatureを編集している状態でmainのバグに気づいた時、bugfixブランチを作成しデバックする。
> 
> ![画像](/images/contributing/exp_debug.png)
> 
> 1. 現在の変更をstashし、保存する。
> 2. 最新のmainブランチにcheckoutし、bugfixブランチを作成する。
> 3. ブランチを更新し、コミットする。
> 4. 作業が完了したらmainにマージする。
> 5. 共同者がmainを更新していたらpull requestの処理に準拠する。
> 6. 元のfeatureブランチに戻りstashを展開すると作業を再開できる。
> 
> 上の手順を行うことで下図の状態となる。
>
> ![画像](/images/contributing/exp_debug_cleate_branch.png)
> ![画像](/images/contributing/exp_debug_after.png)

> ### バグ修正など一部のコミットだけ取り込む（cherry-pick）
> mainブランチでバグ修正を行いそのコミットを自分のブランチに取り込む場合、
> マージするとmainで行ったすべての変更が取り込まれてしまい、開発途中で大きな衝突が発生する。<br>
> 取り込むコミットが一つだけの場合git cherry-pick (コミット値)で取り込む。
> 
> 下図のfeatureでbugfixを取り込む場合、コミット値2、3のものだけ取り込む必要がある。
> 
> ![画像](/images/contributing/exp_cherrypick.png)
> 
> 1. 現在の変更をstashし、保存する。
> 2. featureブランチのまま **`git cherry-pick 1..3`** でコミット2、3を取り込む
> 3. 途中で衝突が発生した場合、修正を行いステージング状態にする。**`（絶対にコミットしない）`**
> 4. 衝突処理が完了したら **`git cherry-pick --continue`** でチェリーピックを再開する。
> 
> 上の手順を行うことで下図の状態となる。（実際はコミット値が変わる）
> 
> ![画像](/images/contributing/exp_cherrypick_after.png)
> 
> 複数のコミットを取り込むとチェリーピックしたことが分かりづらいため、
> 一つのコミットにまとめる。<br>
> 
> 5. **`git reset --soft HEAD~(個数)`** でコミットをステージング状態に戻す。
> 6. コミットし直してチェリーピックしたことが分かりやすい名前にする。
> 7. stashを展開すると作業を再開できる。
> 
> ![画像](/images/contributing/exp_reset.png)

> ### pull requestする
> pull requestはブランチを作った時点で空のコミットを作り **`draft pull request`** する。<br>
> 詳細なdraft pull requestの方法は[こちら](#draft-pull-requestの方法)
</details>

---

<a id="githubの使い方"></a>
<details>
<summary><span style="font-size:1.5em; font-weight:bold;">githubの使い方</span></summary>

---

このリポジトリでは、GitHubの機能を以下の役割で使い分けます。

## 1. Issues

- バグ、改善案、作業タスクを記録
- 再現手順、期待動作、関連ファイルを必ず記載
- 仕様変更はIssue化してから着手

詳しくは [issueの使い方](/docs/issue_usage.md) を参照。

## 2. Pull Requests

- ブランチ作成直後に Draft Pull Request を作成
- 実装途中からPR上で進捗を共有
- マージ前に最終チェック（差分、説明、ドキュメント更新）

詳しくは [Draft-Pull-Requestの方法](/docs/draft_pull_request.md) を参照。

## 3. Discussions

- 設計案、運用方針、アイデア相談の場として利用
- Issue化する前の論点整理に使う

## 4. Actions

- 現在の主要自動化は `docs-pages.yml`
- `.github/libraries.json` をもとに C++/Python ライブラリのドキュメントを生成
- 生成サイトを GitHub Pages にデプロイ

## 5. Wiki（任意）

- 長期的に残したい知見や調査ログの保存先として利用可能
</details>

---

<a id="Draft-Pull-Requestの方法"></a>
<details>
<summary><span style="font-size:1.5em; font-weight:bold;">Draft-Pull-Requestの方法</span></summary>

---

このリポジトリでは、開発開始時点でDraft PRを作る運用を基本とする。

## 目的

- 進捗の見える化
- 設計判断の履歴化
- 途中レビューによる手戻り削減
- 開発logの記録

## 手順

下図の状態から新しくfeatureブランチを作成し、開発する。<br>
![画像](/images/contributing/exp_pullrequest.png)

featureブランチに空のコミットを追加し、draft pull requestを作成する。<br>
![画像](/images/contributing/exp_pullrequest_cleate_branch.png)

featureで開発を進める。（共同者も並行して開発するため、mainブランチは更新される）<br>
共同者が読みやすいようにコード整理を行う。<br>
![画像](/images/contributing/exp_pullrequest_commit.png)

featureブランチにmainブランチをマージし、最新の状態を取り込む。<br>
取り込んだ際に衝突やバグが発生したら、解決してコミットする。（ブランチを新しく分けてもいい）<br>
![画像](/images/contributing/exp_pullrequest_merge.png)

mainブランチにfeatureブランチをマージする。<br>
mainブランチが更新されていたら再び最新のmainブランチをマージしてからfeatureブランチをマージする。<br>
![画像](/images/contributing/exp_pullrequest_after.png)
</details>

---

<a id="issueの使い方"></a>
<details>
<summary><span style="font-size:1.5em; font-weight:bold;">issueの使い方</span></summary>

---

Issueは「実装タスク」だけでなく「仕様確認」も含めて管理する。

## 起票のルール

- 1Issue1目的にする
- タイトルは具体的に書く
- 再現手順や期待動作を必ず書く
- 影響範囲（ファイル、機能）を明記する

## 推奨テンプレート

```md
## 背景

## 目的

## 現状

## 期待する状態

## 作業内容
- [ ]
- [ ]

## 関連情報
- PR:
- 参考リンク:
```

## 使い分け

- バグ: 再現手順・ログ・期待結果を重視
- 機能追加: 目的・受け入れ条件を重視
- ドキュメント更新: 対象ファイルと更新意図を重視

## 運用

- 実装を開始したら関連PRをリンクする
- 完了時はチェックリストを埋め、結果をコメントする
</details>

---

<a id="環境構築"></a>
<details>
<summary><span style="font-size:1.5em; font-weight:bold;">環境構築</span></summary>

---

このプロジェクトは Dev Container 前提です。  
まずコンテナを起動し、次にGitHub連携を設定してください。  
コンテナを初めてビルドする際は数十分かかることがあります。

> ### dockerの環境構築
> ### 1. Docker Desktopをインストール
> **`Docker Desktop`** の公式サイトは[こちら](https://www.docker.com/products/docker-desktop/)<br>
> dockerを使用する際は **`Docker Desktop`** を起動する必要がある。<br>
> 一回起動したら閉じても大丈夫<br>
> 
> ### 2. vscodeの拡張機能 Dev Container をインストール
> Dev Containerの識別子は **`ms-vscode-remote.remote-containers`** なので検索欄に入れたら出てくる。<br>
> vscodeなら左下の青い部分をクリックすることで **`コンテナで再度開く`** 選択肢が出てくる<br>
> .devcontainerに複数フォルダ作られてたら自分に適した環境を選択<br>
> - `.devcontainer/generic/devcontainer.json`
> 	- CPU作業向け（通常はこちら）
> - `.devcontainer/nvidia/devcontainer.json`
> 	- CUDA/GPU検証向け
> 
> コンテナの環境構築は **`Dockerfile`** 、**`devcontainer.json`** にかく

> ### gitの環境構築
> ### 1. linuxターミナルで以下を実行する
> ```bash
> sudo apt update
> sudo apt install gh -y
> gh auth login
> ```
> 
> ### 2. ４つの項目を聞かれるので以下を選択
> ```txt
> what account do you want to log into?
> > GitHub.com
> 
> what is your preferred protocol for Git operations?
> > HTTPS
> 
> Authenticate Git with your GitHub credentials?
> > y
> 
> How would you like to authenticate GitHub CLI?
> > Login with a web browser
> ```
> 
> ### 3. git pushを行えたら成功
>
> ### gitの設定
> 以下のコマンドを実行し、設定すると便利<br>
> **`user.name`** と **`user.email`** は必須
> 
> ```bash
> git config --global user.name "Your Name"
> git config --global user.email "your-email@example.com"
> git config --global push.autoSetupRemote true
> git config --global push.default current
> git config --global core.editor "code --wait"
> git config --global merge.conflictstyle diff3
> ```
> 
> **`git config --global --list`** で設定一覧を表示できる。
</details>

---

<a id="リポジトリ設定"></a>
<details>
<summary><span style="font-size:1.5em; font-weight:bold;">リポジトリ設定</span></summary>

---

本リポジトリはテンプレート利用を想定しているため、設定は「運用自動化」を優先する。

## 推奨設定

### 1. ブランチ自動削除

- `Settings > General > Pull Requests`
- `Automatically delete head branches` を有効化

### 2. GitHub Pages（Actions連携）

- `Settings > Pages`
- Source を `GitHub Actions` に設定
- `.github/workflows/docs-pages.yml` からのデプロイを有効化

### 3. テンプレート運用

- このリポジトリをテンプレートとして参照し、案件ごとに別リポジトリを作成する

### 4. 保守ルール

- `docs/` を更新したらコミット時にフックで目次・`CONTRIBUTING.md`を同期する
- ライブラリ公開対象を変える場合は `.github/libraries.json` を更新する
</details>

---

<a id="Dockerの使い方"></a>
<details>
<summary><span style="font-size:1.5em; font-weight:bold;">Dockerの使い方</span></summary>

---

このリポジトリは Docker + Dev Containers を標準運用とします。

## 使い分け

- `generic`:
	- `.devcontainer/generic/devcontainer.json`
	- CPU前提の共通環境
	- 日常開発、ドキュメント更新、通常の検証向け
- `nvidia`:
	- `.devcontainer/nvidia/devcontainer.json`
	- CUDA対応GPUを使う検証向け
	- `BASE_IMAGE=nvidia/cuda:12.6.0-devel-ubuntu22.04`
	- `--gpus all` などのGPU関連runArgsあり

## 起動手順

1. Docker Desktopを起動
2. VS Codeでリポジトリを開く
3. `Dev Containers: Reopen in Container` を実行
4. `generic` または `nvidia` を選ぶ
</details>

---

<a id="testerのドキュメント"></a>
<details>
<summary><span style="font-size:1.5em; font-weight:bold;">testerのドキュメント</span></summary>

---

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

- Python 共通設定: `tester/python/conftest.py`
- Python テスト実装: `tester/python/unit/library/test_array.py`
- C++ テスト実装: `tester/cpp/unit/library/array_test.cpp`
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
</details>

---

<a id="copilotのドキュメント"></a>
<details>
<summary><span style="font-size:1.5em; font-weight:bold;">copilotのドキュメント</span></summary>

---
</details>

---
