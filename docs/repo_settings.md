<!-- BEGIN-NAVIGATION -->
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
- [リポジトリ設定](/docs/repo_settings.md) - now
- [Dockerの使い方](/docs/docker_usage.md)
    - [testerのドキュメント](/docs/tester-docs/README.md)
    - [copilotのドキュメント](/docs/copilot-docs/github-copilot-cli-guide.md)

## リポジトリ設定
<!-- END-NAVIGATION -->

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
