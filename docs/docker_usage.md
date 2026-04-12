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
- [環境構築](/docs/environment_setup.md)
- [リポジトリ設定](/docs/repo_settings.md)
- [Dockerの使い方](/docs/docker_usage.md) - now

## Dockerの使い方
<!-- END-NAVIGATION -->

このリポジトリは Docker + Dev Containers を標準運用とします。

## 使い分け

- `generic`:
	- `.devcontainer/generic/devcontainer.json`
	- CPU前提の共通環境
	- 日常開発、ドキュメント更新、通常の検証向け
- `nvidia`:
	- `.devcontainer/nvidia/devcontainer.json`
	- CUDA対応GPUを使う検証向け
	- `BASE_IMAGE=nvidia/cuda:12.6.0-devel-ubuntu22.04`
	- `--gpus all` などのGPU関連runArgsあり

## 起動手順

1. Docker Desktopを起動
2. VS Codeでリポジトリを開く
3. `Dev Containers: Reopen in Container` を実行
4. `generic` または `nvidia` を選ぶ
