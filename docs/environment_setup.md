<!-- BEGIN-NAVIGATION -->
# 目次
- [フォルダ構成](./directory_structure.md)
- [基本的なクラス設計](./class_design.md)
- [コーディング時の注意](./coding_guidelines.md)
- [名前の定義ルール](./naming_rules.md)
- [gitの使い方](./git_usage.md)
- [開発フロー](./development_flow.md)
- [githubの使い方](./github_usage.md)
- [Draft-Pull-Requestの方法](./draft_pull_request.md)
- [issueの使い方](./issue_usage.md)
- [環境構築](./environment_setup.md) - now
- [リポジトリ設定](./repo_settings.md)
- [Dockerの使い方](./docker_usage.md)

## 環境構築
<!-- END-NAVIGATION -->

> ### 1. linuxターミナルで以下を実行する
> ```bash
> sudo apt update
> sudo apt install gh -y
> gh auth login
> ```
> 
> ### 2. ４つの項目を聞かれるので以下を選択
> ```txt
> what account do you want to log into?
> > GitHub.com
> 
> what is your preferred protocol for Git operations?
> > HTTPS
> 
> Authenticate Git with your GitHub credentials?
> > y
> 
> How would you like to authenticate GitHub CLI?
> > Login with a web browser
> ```
> 
> ### 3. git pushを行えたら成功

> ### gitの設定
> 以下のコマンドを実行し、設定すると便利
> 
> ```bash
> git config --global user.name "Your Name"
> git config --global user.email "your-email@example.com"
> git config --global push.autoSetupRemote true
> git config --global push.default current
> git config --global core.editor "code --wait"
> git config --global merge.conflictstyle diff3
> ```
> 
> **`git config --global --list`**で設定一覧を表示できる。
