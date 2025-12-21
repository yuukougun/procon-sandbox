# proconでのリポジトリの使い方
## 目次
- [フォルダ構成](#フォルダ構成)
    - [doxygen.yml](#doxygenymlファイル)
    - [library](#libraryフォルダ)
    - [solve](#solveフォルダ)
    - [server](#serverフォルダ)
    - [visualizer](#visualizerフォルダ)
    - [analysis](#analysisフォルダ)
    - [run.sh](#runshファイル)
- [コーディング時の注意](#コーディング時の注意)
- [名前の定義ルール](#名前の定義ルール)
- [基本的なクラス設計](#基本的なクラス設計)
- pass

<a id="フォルダ構成"></a>
<details>
<summary><span style="font-size:1.5em; font-weight:bold;">フォルダ構成</span></summary>

---
```
repository
├── .github
│   └── workflows
|       ├── test.yml    # pull request時にtesterを実行
│       └── doxygen.yml # ライブラリの設計仕様をpagesにデプロイ
├── .vscode
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

>### `doxygen.yml`ファイル
> libraryフォルダで定義したクラスや関数を **`doxygen`** でドキュメンテーション化し、
> プッシュ時にdoxygen.ymlファイルで **`github pages`** に自動デプロイする。<br>
>
> github pagesのリンクは[こちら](https://google.com)

>### `library`フォルダ
> ドキュメンテーションコメントを書く。**`（処理の説明ではなく、使い方の説明）`**<br>
> solveで使用するc++のクラスや関数をまとめる。<br>
> 関数には **`inline`** をつけてリンクできるようにする<br>
> serverやvisualizerのpythonライブラリを作る場合は **`library / pythonフォルダ内`** に置く。<br>
> ライブラリを作成する前にLIBRARY_DESIN.mdに設計仕様をまとめ、
> 計画的にクラスを作成する。 **`（他の人と相談しながらがいい）`**<br>
> umbrella headerを作成し、一括includeできるようにする。<br>
> #pragma onceを用いて **`インクルードガード`** する。
>
>---
> #### ライブラリ作成の注意
> 名前は長くていいから絶対に **`今後被らないような名前`** にする。（名前の定義ルールは[こちら](#名前の定義ルール)）<br>
> ライブラリファイルは **`基本変更しない`** ような設計にする。（パラメータ値はsolveフォルダ内に書きincludeする）<br>
> 例外処理か、エラー出力を行う。エラー出力では内部データを出力し、**`デバックしやすくする`**。<br>
> 右辺値や左辺値、constを意識して **`無駄なコピー`** が発生しないようなクラスにする。<br>
> 役割ごとに **`namespace`** を分割し、一つのファイルに多くのクラスや関数を定義しない。<br>
> クラス内に構造体が必要になったら **`クラス内に書く`** 。<br>
> クラス内のメンバ変数はpublicにせず、getterやsetterでやり取りする。<br>
> 同じ関数でも引数を変えてオーバーライドし、様々な状況で使いやすくする。<br>
> templateを用いて柔軟なクラスにする。（競技のデータ構造による）<br>
> conceptやrequiresを用いて **`templateの型を制限する`**。<br>
> コンストラクタには基本explicitをつけ、暗黙の型変換を防止する。<br>
> コンパイル時に計算できる可能性のある関数は基本contexprをつける<br>
>
> クラスの基本的な設計は[こちら](#基本的なクラス設計)

>### `solve`フォルダ
> 大まかな解法ごとにフォルダをネストさせる。<br>
> 処理ごとにutil.hppでファイルを分け、includeする。（ライブラリフォルダではなくsolveフォルダに入れる）<br>
> 似た解法のsolve.cppを作るときは同じutil.hppをincludeして共通の処理を一つのファイルで管理<br>
> 処理ごとに関数に分け、関数の役割を分散する。<br>
> パラメータはsolveファイルに書かず、hppに分け、定数値として定義する。


>### `server`フォルダ
> 大会サーバーやpc間の通信処理はここで行う。<br>
> guiのボタンで簡単に操作できるようにする。

>### `visualizer`フォルダ
> 回答データや分析データをguiで可視化する。<br>
> run.shと被るが、solveを実行しそれぞれの入力例を入力できるようにすると便利

>### `tester`フォルダ
> コンパイルできるか、関数やクラスが適切な値を返すか、短時間で確認できるテストを試す。<br>
> テスト方法は要検討

>### `analysis`フォルダ
> 回答データを分析し、推移や解法ごとの分析データを保存する<br>
> 状態変化をgifとして保存するのもあり

>### `run.sh`ファイル
> ファイルのコンパイルや実行などターミナルで実行するコマンド処理は全てこれで行う。（gitコマンドは除く）<br>
> makefileでオブジェクトファイルの作成やリンクもこれで行う。

</details>

---

<a id="コーディング時の注意"></a>
<details>
<summary><span style="font-size:1.5em; font-weight:bold;">コーディング時の注意</span></summary>

---
- [名前の定義ルール](#名前の定義ルール)を守る。変数や関数などの名前は絶対に日本語にしない！！
- あやふやな名前、被りそうな名前は使わない、長くてもいいから誰でも意味が分かる名前にする。
- 一時的な変数は以下のように書き、延命を防ぐ。
```c++
int main(){
    int n = 1;
    {
        int tmp = n * 2;
        cout << tmp << endl;
    }
}
```
- {}内の処理が長くなる場合は以下のように最後にコメントを書く。
```c++
namespace test{
    // ...処理...
}//namespace test
```
- コメントを書く。
- 頻繁にコミットする。
- 処理を追加するときはブランチを切る。
- testerをちゃんと作る
- [フォルダ構成](#フォルダ構成)に書かれたファイル名を厳守する必要はない、役割に合わせて柔軟に変更。

</details>

---

<a id="名前の定義ルール"></a>
<details>
<summary><span style="font-size:1.5em; font-weight:bold;">名前の定義ルール</span></summary>

---
> ### 一般的な命名規則として５つに分類される。
> 1. パスカルケース（MyName）
> 2. キャメルケース（myName）
> 3. スクリーミングケース（MY_NAME）
> 4. スネークケース（my_name）
> 5. ケバブケース（my-name）

> ### 種類別の命名規則
> | 種類 | 命名規則 |
> | --- | --- |
> | 型 | パスカルケース（MyName） |
> | ファイル、フォルダ | スネークケース（my_name） |
> | クラス | パスカルケース（MyName） |
> | 関数、メソッド | スネークケース（my_name） |
> | 変数 | スネークケース（my_name） |
> | 定数 | スクリーミングケース（MY_NAME） |
> | 真偽値（bool）| is_my_name |

</details>

---

<a id="基本的なクラス設計"></a>
<details>
<summary><span style="font-size:1.5em; font-weight:bold;">基本的なクラス設計</span></summary>

---
```c++
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

> クラスがvectorに似た挙動をするならvectorに使われている関数をあらかた実装する。<br>
> 標準ライブラリのリファレンスは[こちら](https://cpprefjp.github.io/reference.html)

</details>

---

<a id="gitの使い方"></a>
<details>
<summary><span style="font-size:1.5em; font-weight:bold;">gitの使い方</span></summary>
gitの操作は基本ターミナルで行う。commitなどはvscodeのguiで行ったほうがいい<br>
commitしてない状態でブランチを変えることはできない。<br>
commitメッセージは日本語で分かりやすく書く<br>
複数の人が同じブランチを編集している状態は避ける。<br>
マージしたあとはブランチを削除する。（ブランチの履歴は消えない）<br>
stashするときはすべてのファイルを保存する。<br>
stashはいくつも作れるが、基本一つにする。<br>
mergeはせずにpull requestを送る。ymlが自動判定するから成功したら自分でpull requestを許可する。

---
> ### gitコマンド一覧
> 共同開発で使うものだけを一覧にしたため、他のコマンドは使わないと思う。<br>
> より多くのgitコマンドは[こちら](https://zenn.dev/zmb/articles/054ba4189244a5) 
>
> | コマンド | 効果 |
> | --- | --- |
> | git status | 現在の状態を表示 |
> | git add (ファイル名) | (ファイル名)をステージングする |
> | git add . | すべてのファイルをステージングする |
> | git commit | コミットする（vscodeのguiでやったほうがいい）|
> | | |
> | git push | 現在のブランチのローカルの変更内容をリモートに送信 |
> | git push origin (ブランチ名) | (ブランチ名)のローカルの変更内容をリモートに送信 |
> | git pull | 現在のブランチのリモートの変更内容をローカルに取り込む |
> | git pull origin (ブランチ名) | (ブランチ名)のリモートの変更内容をローカルに取り込む |
> | git pull --rebase | 自分のcommitを他人のcommitの後に変える |
> |  |  |
> | git branch | ブランチの一覧を表示 |
> | git checkout (ブランチ名) | (ブランチ名)に切り替え |
> | git checkout (コミット値) | (コミット値)に切り替え |
> | git checkout -b (ブランチ名) | (ブランチ名)のブランチを作成 |
> |  |  |
> | git stash | 編集した内容を退避 |
> | git stash list | stashの一覧を表示（stashの番号はこれで確認）|
> | git stash show stash@{(番号)} | (番号)番目のstashを詳細表示 |
> | git stash save (コメント) | stashに(コメント)をつけてstashする |
> | git stash pop | 編集した内容を呼び出す |
> | git stash pop stash@{(番号)} | (番号)番目のstashでpopする |
> | git stash drop stash@{(番号)} | (番号)番目のstashを削除 |
> |  |  |
> |  |  |
> |  |  |
> |  |  |
> |  |  |
> | git restore (ファイル名) | (ファイル名)で編集した内容を破棄（超危険） |
> | git restore . | すべてのファイルで編集した内容を破棄（超危険） |
> |  |  |
> |  |  |
> | git reflog | git操作の状態履歴を表示 |
> | git reset --hard HEAD@{(番号)} | リポジトリを(番号)番目の状態に戻す（超危険） |
> |  |  |



</details>

---
ギットのチェリーピックのテスト
ギットのチェリーピックのテスト2