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
    - [testerのドキュメント](/docs/tester-docs/README.md)
    - [copilotのドキュメント](/docs/copilot-docs/github-copilot-cli-guide.md)

## コーディング時の注意
<!-- END-NAVIGATION -->

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
