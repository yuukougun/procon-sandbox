<!-- BEGIN-NAVIGATION -->
# 目次 [home](/CONTRIBUTING.md)
- [ディレクトリ構成](/docs/directory_structure.md)
- [基本的なクラス設計](/docs/class_design.md)
- [コーディング時の注意](/docs/coding_guidelines.md) - now
- [名前の定義ルール](/docs/naming_rules.md)
- [gitの使い方](/docs/git_usage.md)
- [開発フロー](/docs/development_flow.md)
- [githubの使い方](/docs/github_usage.md)
- [Draft-Pull-Requestの方法](/docs/draft_pull_request.md)
- [issueの使い方](/docs/issue_usage.md)
- [環境構築](/docs/environment_setup.md)
- [リポジトリ設定](/docs/repo_settings.md)
- [Dockerの使い方](/docs/docker_usage.md)

## コーディング時の注意
<!-- END-NAVIGATION -->

- [名前の定義ルール](#名前の定義ルール)を守る。変数や関数などの名前は **`絶対に日本語にしない！！`**
- あやふやな名前、被りそうな名前は使わない、長くてもいいから誰でも **`意味が分かる名前`** にする。
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
- testerをちゃんと作る。
- [フォルダ構成](#フォルダ構成)に書かれたファイル名を厳守する必要はない、役割に合わせて柔軟に変更。
