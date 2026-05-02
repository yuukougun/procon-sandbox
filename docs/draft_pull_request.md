<!-- BEGIN-NAVIGATION -->
# 目次 [home](/CONTRIBUTING.md)
- [ディレクトリ構成](/docs/directory_structure.md)
- [基本的なクラス設計](/docs/class_design.md)
- [コーディング時の注意](/docs/coding_guidelines.md)
- [名前の定義ルール](/docs/naming_rules.md)
- [gitの使い方](/docs/git_usage.md)
- [開発フロー](/docs/development_flow.md)
- [githubの使い方](/docs/github_usage.md)
- [Draft-Pull-Requestの方法](/docs/draft_pull_request.md) - now
- [issueの使い方](/docs/issue_usage.md)
- [環境構築](/docs/environment_setup.md)
- [リポジトリ設定](/docs/repo_settings.md)
- [Dockerの使い方](/docs/docker_usage.md)
    - [testerのドキュメント](/docs/tester-docs/README.md)
    - [copilotのドキュメント](/docs/copilot-docs/github-copilot-cli-guide.md)

## Draft-Pull-Requestの方法
<!-- END-NAVIGATION -->

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
