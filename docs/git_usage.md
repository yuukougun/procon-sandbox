<!-- BEGIN-NAVIGATION -->
# 目次 [home](/CONTRIBUTING.md)
- [ディレクトリ構成](/docs/directory_structure.md)
- [基本的なクラス設計](/docs/class_design.md)
- [コーディング時の注意](/docs/coding_guidelines.md)
- [名前の定義ルール](/docs/naming_rules.md)
- [gitの使い方](/docs/git_usage.md) - now
- [開発フロー](/docs/development_flow.md)
- [githubの使い方](/docs/github_usage.md)
- [Draft-Pull-Requestの方法](/docs/draft_pull_request.md)
- [issueの使い方](/docs/issue_usage.md)
- [環境構築](/docs/environment_setup.md)
- [リポジトリ設定](/docs/repo_settings.md)
- [Dockerの使い方](/docs/docker_usage.md)
    - [testerのドキュメント](/docs/tester-docs/README.md)
    - [copilotのドキュメント](/docs/copilot-docs/github-copilot-cli-guide.md)

## gitの使い方
<!-- END-NAVIGATION -->

## 基本方針

- `main` への直接コミットは避け、原則PR経由でマージする
- 1ブランチ1目的で作業する
- コミットメッセージは日本語で、変更意図が分かるように書く
- ブランチはdraft PRを作成してから切る
- コマンド実行時に途中で衝突したら実行が止まるため、--continueで再開
- コマンドを途中で中断するときは、--abort

> ### gitコマンド一覧
> 共同開発で使うものだけを一覧にしたため、他のコマンドは使わないと思う。<br>
> より多くのgitコマンドは[こちら](https://zenn.dev/zmb/articles/054ba4189244a5) 
> git用語は[こちら](https://qiita.com/shinshingodmt/items/637cf9e5c6660509c460)
>
> | コマンド | 効果 |
> | --- | --- |
> | git status | 現在の状態を表示 |
> |  |  |
> | git add (ファイル名) | (ファイル名)をステージングする |
> | git add . | すべてのファイルをステージングする |
> | git commit -m (コミットメッセージ) | ステージングファイルをコミットする |
> | git commit -n -m (コミットメッセージ) | pre-commitフックを実行せずにコミットする |
> | git commit --amend -m (コミットメッセージ) | 直前のコミットメッセージを変更（ステージング状態は空にしておく） |
> | git commit --allow-empty -m "空のコミット" | 空のコミットを作成する |
> | git merge (ブランチ名) | (ブランチ名)を現在のブランチにマージ（mainにはpull requestする）|
> | git merge --continue | mergeの再開 |
> |  |  |
> | git push | 現在のブランチのローカルの変更内容をリモートに送信 |
> | git push (ブランチ名) | (ブランチ名)のローカルの変更内容をリモートに送信 |
> | git push -u origin (ブランチ名) | リモートに(ブランチ名)を追加してpushする |
> | git push origin -d (ブランチ名) | リモートの(ブランチ名)を削除 |
> | git pull | 現在のブランチのリモートの変更内容をローカルに取り込む |
> | git pull origin (ブランチ名) | (ブランチ名)のリモートの変更内容をローカルに取り込む |
> | git pull --rebase | 自分のcommitを他人のcommitの後に変える |
> | git fetch --all | ローカルを最新の状態に更新（ローカルリポジトリは変更しない）|
> |  |  |
> | git branch | ブランチの一覧を表示 |
> | git branch -m (現在のブランチ名) (新規ブランチ名) | ブランチ名の変更 |
> | git branch -d (ブランチ名) | (ブランチ名)を削除 |
> | git branch -D (ブランチ名) | マージしてない(ブランチ名)を削除（超危険）|
> | git checkout (ブランチ名) | (ブランチ名)に切り替え |
> | git checkout (コミット値) | (コミット値)に切り替え |
> | git checkout -b (ブランチ名) | (ブランチ名)のブランチを作成 |
> | git checkout (ブランチ名) -- (ファイル名) | (ブランチ名)から(ファイル名)をコピーしadd |
> |  |  |
> | git stash | 編集した内容を退避 |
> | git stash -u | 新規作成したファイルも含めて編集した内容を退避 |
> | git stash -a | 追跡対象外のファイルも含めて編集した内容を退避 |
> | git stash list | stashの一覧を表示（stashの番号はこれで確認）|
> | git stash show stash@{(番号)} | (番号)番目のstashを詳細表示 |
> | git stash save (コメント) | stashに(コメント)をつけてstashする |
> | git stash pop | 編集した内容を呼び出す |
> | git stash pop stash@{(番号)} | (番号)番目のstashでpopする |
> | git stash drop stash@{(番号)} | (番号)番目のstashを削除（超危険） |
> |  |  |
> | git rebase (ブランチ名) | (ブランチ名)にrebaseする |
> | git rebase --continue | rebaseの再開 |
> | git cherry-pick (コミット値) | (コミット値)を取り込む |
> | git cherry-pick (コミット値1)..(コミット値2) | (コミット値1)の次から(コミット値2)を順にcherry-pick |
> | git cherry-pick --continue | cherry-pickの再開 |
> |  |  |
> | git revert HEAD | 直前のコミットを打ち消すコミットを追加 |
> | git revert (コミット値) | (コミット値)のコミットを打ち消すコミットを追加 |
> | git revert -m 1 (マージのコミット値) | マージを打ち消すコミットを追加 |
> | git restore (ファイル名) | (ファイル名)で編集した内容を破棄（超危険） |
> | git restore . | すべてのファイルで編集した内容を破棄（超危険） |
> |  |  |
> | git reflog | git操作の状態履歴を表示（履歴の番号はこれで確認） |
> | git reset --soft ORIGIN_HEAD | マージ、リベース、リセットの処理をもとに戻す（危険） |
> | git reset --soft HEAD^ | 直近１個のコミットをステージング状態に戻す。（危険）|
> | git reset --soft HEAD~(個数) | 直近(個数)個のコミットをステージング状態に戻す（危険） |
> | git reset --soft (コミット値) | (コミット値)の状態に戻す（危険） |
> | git reset --soft HEAD@{(番号)} | リポジトリを(番号)番目の状態に戻す（危険） |
> |  |  |
> | git tag | タグの一覧を表示 |
> | git tag -a (タグ名) -m "(タグのコメント)" | 直近のコミットにタグを付ける |
> | git tag -a (タグ名) -m "(タグのコメント)" (コミット値) | (コミット値)にタグを付ける |
> | git tag -d (タグ名) | タグを削除 |
