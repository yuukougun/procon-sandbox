<!-- BEGIN-NAVIGATION -->
# 目次
- [フォルダ構成](./directory_structure.md)
- [基本的なクラス設計](./class_design.md) - now
- [コーディング時の注意](./coding_guidelines.md)
- [名前の定義ルール](./naming_rules.md)
- [gitの使い方](./git_usage.md)
- [開発フロー](./development_flow.md)
- [githubの使い方](./github_usage.md)
- [draft-pull-requestの方法](./draft_pull_request.md)
- [issueの使い方](./issue_usage.md)
- [環境構築](./environment_setup.md)
- [リポジトリ設定](./repo_settings.md)
- [Dockerの使い方](./docker_usage.md)

## 基本的なクラス設計
<!-- END-NAVIGATION -->

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
