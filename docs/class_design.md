<!-- BEGIN-NAVIGATION -->
# 目次 [home](/CONTRIBUTING.md)
- [ディレクトリ構成](/docs/directory_structure.md)
- [基本的なクラス設計](/docs/class_design.md) - now
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

## 基本的なクラス設計
<!-- END-NAVIGATION -->

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
