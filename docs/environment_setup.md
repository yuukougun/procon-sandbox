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
- [環境構築](/docs/environment_setup.md) - now
- [リポジトリ設定](/docs/repo_settings.md)
- [Dockerの使い方](/docs/docker_usage.md)

## 環境構築
<!-- END-NAVIGATION -->

このプロジェクトは Dev Container 前提です。  
まずコンテナを起動し、次にGitHub連携を設定してください。  
コンテナを初めてビルドする際は数十分かかることがあります。

> ### dockerの環境構築
> ### 1. Docker Desktopをインストール
> **`Docker Desktop`** の公式サイトは[こちら](https://www.docker.com/products/docker-desktop/)<br>
> dockerを使用する際は **`Docker Desktop`** を起動する必要がある。<br>
> 一回起動したら閉じても大丈夫<br>
> 
> ### 2. vscodeの拡張機能 Dev Container をインストール
> Dev Containerの識別子は **`ms-vscode-remote.remote-containers`** なので検索欄に入れたら出てくる。<br>
> vscodeなら左下の青い部分をクリックすることで **`コンテナで再度開く`** 選択肢が出てくる<br>
> .devcontainerに複数フォルダ作られてたら自分に適した環境を選択<br>
> - `.devcontainer/generic/devcontainer.json`
> 	- CPU作業向け（通常はこちら）
> - `.devcontainer/nvidia/devcontainer.json`
> 	- CUDA/GPU検証向け
> 
> コンテナの環境構築は **`Dockerfile`** 、**`devcontainer.json`** にかく

> ### gitの環境構築
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
>
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

## 6. 動作確認
