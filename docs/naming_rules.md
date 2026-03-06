<!-- BEGIN-NAVIGATION -->
# 目次
- [フォルダ構成](./directory_structure.md)
- [基本的なクラス設計](./class_design.md)
- [コーディング時の注意](./coding_guidelines.md)
- [名前の定義ルール](./naming_rules.md) - now
- [gitの使い方](./git_usage.md)
- [開発フロー](./development_flow.md)
- [githubの使い方](./github_usage.md)
- [Draft-Pull-Requestの方法](./draft_pull_request.md)
- [issueの使い方](./issue_usage.md)
- [環境構築](./environment_setup.md)
- [リポジトリ設定](./repo_settings.md)
- [Dockerの使い方](./docker_usage.md)

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
> ブランチ名の前にそのブランチの種類を書く（例：bugfix/library）
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
