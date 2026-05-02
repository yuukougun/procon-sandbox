<!-- BEGIN-NAVIGATION -->
# 目次 [home](/CONTRIBUTING.md)
- [ディレクトリ構成](/docs/directory_structure.md)
- [基本的なクラス設計](/docs/class_design.md)
- [コーディング時の注意](/docs/coding_guidelines.md)
- [名前の定義ルール](/docs/naming_rules.md)
- [gitの使い方](/docs/git_usage.md)
- [開発フロー](/docs/development_flow.md) - now
- [githubの使い方](/docs/github_usage.md)
- [Draft-Pull-Requestの方法](/docs/draft_pull_request.md)
- [issueの使い方](/docs/issue_usage.md)
- [環境構築](/docs/environment_setup.md)
- [リポジトリ設定](/docs/repo_settings.md)
- [Dockerの使い方](/docs/docker_usage.md)
    - [testerのドキュメント](/docs/tester-docs/README.md)
    - [copilotのドキュメント](/docs/copilot-docs/github-copilot-cli-guide.md)

## 開発フロー
<!-- END-NAVIGATION -->

git用語は[こちら](https://qiita.com/shinshingodmt/items/637cf9e5c6660509c460)

- 進捗共有はDraft Pull Requestを起点にする
- 変更は小さく、頻繁にpushする
- 基本的に開発時のメモは[discussionやwiki](#githubの使い方)を使う
- 一旦の数値等の一時的なメモはmemoファイルに書く

## 目次
- [図の説明](#図の説明)
- [リモートとローカルの違い](#リモートとローカルの違い)
- [stashの使い方](#stashの使い方)
- [同じブランチで共同者がコミットしていた時の対処（rebase）](#同じブランチで共同者がコミットしていた時の対処（rebase）)
- [バグや改善を発見した場合の対処](#バグや改善を発見した場合の対処)
- [バグ修正など一部のコミットだけ取り込む（cherry-pick）](#バグ修正など一部のコミットだけ取り込む（cherry-pick）)
- [pull requestする](#pull-requestする)

> ### 図の説明
> ![画像](/images/contributing/explanation.png)
> 
> - リモートのコミットは枠線
> - ローカルのコミットは塗りつぶし
> - 塗り潰し、枠線の色でブランチの違い
> - ステージング、ステージング前の一時的な変更は灰色
> - コミット値はコミット内の数値
> - 空のコミットはコミット内に空

> ### リモートとローカルの違い
> - ローカルで編集しただけでは、リモートには反映されないため、pushする。<br>
> - 共同者も同時に編集するため、リモートは常に更新される。<br>
> - ローカルの編集とリモートの編集が衝突しないよう、リモートに **`pushする前に必ずfetch`** を行う。<br>
> - なにかリモートに変更を加えるときは、最新のリモートの状態を把握する。<br>
> - ローカルで大量の編集を貯めず、頻繁にpushする。

> ### stashの使い方
> stashコマンドの一覧は[こちら](#gitの使い方)<br>
> 自分のブランチで修正している時に別のブランチを編集したいとき、
> stashコマンドを用いることでステージング、ステージング前の変更がすべて保存される。<br>
> 保存した変更を別のブランチで展開することもできるが、なるべく使用を避ける。
>
> 下図のようにstashと一時的な変更が衝突した時、ステージング状態にしてから展開する。
> 
> ![画像](/images/contributing/exp_stash.png)
> 
> 1. 現在の変更をステージングする。
> 2. **`git stash pop`** で展開する。
> 3. 衝突したら処理して **`git stash list`** で確認する。
> 4. stashが消えてなかったら **`git stash drop stash@{(番号)}`** で消す。
> 
> 上の手順を行うことで一時的な変更を一つにまとめれる。

> ### 同じブランチで共同者がコミットしていた時の対処（rebase）
> 下図のようにローカルで編集した内容をpushする際に、リモートで共同者がコミットしていたとき、そのままpushすることはできない。<br>
> このとき、rebaseを行い自分の変更をリモートブランチの後ろにコミットし直す。
> 
> ![画像](/images/contributing/exp_rebase.png)
> 
> 1. ローカルで行った編集をコミットする。（stashで後から復元すると衝突処理がめんどい）
> 2. **`git rebase origin/(ブランチ名)`** でリモートブランチにリベースする。
> 3. 途中で衝突が発生した場合、修正を行いステージング状態にする。**`（絶対にコミットしない）`**
> 4. 衝突処理が完了したら **`git rebase --continue`** でリベースを再開する。
>
> 上の手順を行うことで下図の状態となる。（一時的な編集はコミットしない場合）<br>
> 複数人で同じブランチを直接編集することは避ける。
>
> ![画像](/images/contributing/exp_rebase_after.png)

> ### バグや改善を発見した場合の対処
> 自分のブランチで作業している時にバグや修正を発見した場合。<br>
> 自分のブランチのみで発生したものはブランチ内で解決するべきであるが、
> mainブランチなど、すでに共同者が利用している箇所で発生したものは **`ブランチを変えて編集`** する。<br>
> 発見した瞬間に忘れないよう **`issueとして登録`** し、共同者に共有する。（自分では変更できない場合は共同者に任せる）<br>
> 1コミットですむ数行程度の変更なら新しくブランチを分けずmainにそのままコミットする。<br>
> 少し手間がかかる変更はbugfixやfeatureブランチとして分ける。
>
> 下図のfeatureを編集している状態でmainのバグに気づいた時、bugfixブランチを作成しデバックする。
> 
> ![画像](/images/contributing/exp_debug.png)
> 
> 1. 現在の変更をstashし、保存する。
> 2. 最新のmainブランチにcheckoutし、bugfixブランチを作成する。
> 3. ブランチを更新し、コミットする。
> 4. 作業が完了したらmainにマージする。
> 5. 共同者がmainを更新していたらpull requestの処理に準拠する。
> 6. 元のfeatureブランチに戻りstashを展開すると作業を再開できる。
> 
> 上の手順を行うことで下図の状態となる。
>
> ![画像](/images/contributing/exp_debug_cleate_branch.png)
> ![画像](/images/contributing/exp_debug_after.png)

> ### バグ修正など一部のコミットだけ取り込む（cherry-pick）
> mainブランチでバグ修正を行いそのコミットを自分のブランチに取り込む場合、
> マージするとmainで行ったすべての変更が取り込まれてしまい、開発途中で大きな衝突が発生する。<br>
> 取り込むコミットが一つだけの場合git cherry-pick (コミット値)で取り込む。
> 
> 下図のfeatureでbugfixを取り込む場合、コミット値2、3のものだけ取り込む必要がある。
> 
> ![画像](/images/contributing/exp_cherrypick.png)
> 
> 1. 現在の変更をstashし、保存する。
> 2. featureブランチのまま **`git cherry-pick 1..3`** でコミット2、3を取り込む
> 3. 途中で衝突が発生した場合、修正を行いステージング状態にする。**`（絶対にコミットしない）`**
> 4. 衝突処理が完了したら **`git cherry-pick --continue`** でチェリーピックを再開する。
> 
> 上の手順を行うことで下図の状態となる。（実際はコミット値が変わる）
> 
> ![画像](/images/contributing/exp_cherrypick_after.png)
> 
> 複数のコミットを取り込むとチェリーピックしたことが分かりづらいため、
> 一つのコミットにまとめる。<br>
> 
> 5. **`git reset --soft HEAD~(個数)`** でコミットをステージング状態に戻す。
> 6. コミットし直してチェリーピックしたことが分かりやすい名前にする。
> 7. stashを展開すると作業を再開できる。
> 
> ![画像](/images/contributing/exp_reset.png)

> ### pull requestする
> pull requestはブランチを作った時点で空のコミットを作り **`draft pull request`** する。<br>
> 詳細なdraft pull requestの方法は[こちら](#draft-pull-requestの方法)
