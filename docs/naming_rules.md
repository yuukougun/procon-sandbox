<!-- BEGIN-NAVIGATION -->
# 目次 [home](/CONTRIBUTING.md)
- [ディレクトリ構成](/docs/directory_structure.md)
- [基本的なクラス設計](/docs/class_design.md)
- [コーディング時の注意](/docs/coding_guidelines.md)
- [名前の定義ルール](/docs/naming_rules.md) - now
- [gitの使い方](/docs/git_usage.md)
- [開発フロー](/docs/development_flow.md)
- [githubの使い方](/docs/github_usage.md)
- [Draft-Pull-Requestの方法](/docs/draft_pull_request.md)
- [issueの使い方](/docs/issue_usage.md)
- [環境構築](/docs/environment_setup.md)
- [リポジトリ設定](/docs/repo_settings.md)
- [Dockerの使い方](/docs/docker_usage.md)

## 名前の定義ルール
<!-- END-NAVIGATION -->

> ### 一般的な命名規則として５つに分類される。
> 1. パスカルケース（MyName）
> 2. キャメルケース（myName）
> 3. スクリーミングケース（MY_NAME）
> 4. スネークケース（my_name）
> 5. ケバブケース（my-name）

> ### 種類別の命名規則
> | 種類 | 命名規則 |
> | --- | --- |
> | ファイル、フォルダ | スネークケース（my_name） |
> | 型 | パスカルケース（MyName） |
> | クラス | パスカルケース（MyName） |
> | 関数、メソッド | スネークケース（my_name） |
> | 変数 | スネークケース（my_name） |
> | 定数 | スクリーミングケース（MY_NAME） |
> | 真偽値（bool）| is_my_name |

> ### ブランチの命名規則
> ブランチ名には'/'をつけることができる。<br>
> ブランチ名の前にそのブランチの種類を書く（例：bugfix/library）<br>
> 大きなプロジェクトのブランチはmainを増設する。（例：visualizer/main）<br>
> 例：visualizerでbuttonのバグを治すブランチは **`visualizer/bugifx/button`**
> 
> | 種類 | 命名規則 |
> | --- | --- |
> | 機能追加 | feature |
> | バグ修正 | bugfix |
> | コード整理 | refactor |
> | テストコード | test |
> | ドキュメント整理 | docs |
>
> バグの修正でissueを修正する場合は **`bugfix/issue-(issueの番号)`** にする。
