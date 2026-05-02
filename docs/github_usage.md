<!-- BEGIN-NAVIGATION -->
# 目次 [home](/CONTRIBUTING.md)
- [ディレクトリ構成](/docs/directory_structure.md)
- [基本的なクラス設計](/docs/class_design.md)
- [コーディング時の注意](/docs/coding_guidelines.md)
- [名前の定義ルール](/docs/naming_rules.md)
- [gitの使い方](/docs/git_usage.md)
- [開発フロー](/docs/development_flow.md)
- [githubの使い方](/docs/github_usage.md) - now
- [Draft-Pull-Requestの方法](/docs/draft_pull_request.md)
- [issueの使い方](/docs/issue_usage.md)
- [環境構築](/docs/environment_setup.md)
- [リポジトリ設定](/docs/repo_settings.md)
- [Dockerの使い方](/docs/docker_usage.md)

## githubの使い方
<!-- END-NAVIGATION -->

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
