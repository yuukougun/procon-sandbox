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
- [issueの使い方](/docs/issue_usage.md) - now
- [環境構築](/docs/environment_setup.md)
- [リポジトリ設定](/docs/repo_settings.md)
- [Dockerの使い方](/docs/docker_usage.md)

## issueの使い方
<!-- END-NAVIGATION -->

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
