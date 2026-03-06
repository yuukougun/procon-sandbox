<!-- BEGIN-NAVIGATION -->
# 目次
- [フォルダ構成](./directory_structure.md)
- [基本的なクラス設計](./class_design.md)
- [コーディング時の注意](./coding_guidelines.md)
- [名前の定義ルール](./naming_rules.md)
- [gitの使い方](./git_usage.md)
- [開発フロー](./development_flow.md)
- [githubの使い方](./github_usage.md)
- [Draft-Pull-Requestの方法](./draft_pull_request.md) - now
- [issueの使い方](./issue_usage.md)
- [環境構築](./environment_setup.md)
- [リポジトリ設定](./repo_settings.md)
- [Dockerの使い方](./docker_usage.md)

## Draft-Pull-Requestの方法
<!-- END-NAVIGATION -->

ブランチを分け開発が終わってからpull requestを送る方法ではなく、
ブランチを作成した段階でdraft pull requestを送り開発を始める。<br>
pull requestに随時コメントし、共同者に今やっていること、できたことを共有する。

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

