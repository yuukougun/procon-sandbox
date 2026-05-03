# procon-sandbox

高専プロコン向け開発を進めるための、マルチ言語・マルチランタイム前提の開発基盤リポジトリです。  
オセロ実装や学習ロジックは現在も試作・改善中ですが、このリポジトリの主眼は「技術スタックを横断して継続開発できる構成」を整えることにあります。  
本番での開発ではこれをテンプレートリポジトリとし、別のリポジトリで制作できることを目標とする。

git pagesにライブラリのドキュメントを公開している  
https://yuukougun.github.io/procon-sandbox/

> 状態: 制作中（構成・運用ルールは実践に合わせて更新中）  
> オセロAIなどテンプレートリポジトリに不要な実装は最終的に削除する。

## 技術スタック（中心）

| レイヤ | 使用技術 | 役割 |
|---|---|---|
| 言語 | C++, Python, Rust | 高速処理・学習・可視化を分担 |
| 学習/数値計算 | PyTorch, NumPy | 学習 |
| 学習監視 | Weights & Biases | 実験記録、比較 |
| 言語間連携 | pybind11, subprocess | C++エンジンとPython学習系の接続 |
| Web可視化 | Yew, wasm-bindgen, gloo-net, trunk | WASMベースの対局UI |
| Desktop可視化 | eframe/egui | Rust GUI試作 |
| 開発環境 | Docker, Dev Containers | 依存差分の吸収、再現性確保 |
| ドキュメント | Doxygen, Sphinx, GitHub Pages | C++/Pythonライブラリ利用情報の公開 |
| CI/CD | GitHub Actions | ドキュメント生成と自動デプロイ |

## 技術構成の意図

### 1. Dev Containerをgenericとnvidiaで分けている理由

- `generic`: CPU環境で確実に動く共通開発ベース
  - GPU非依存の作業（実装、ドキュメント、一般的な検証）を安定運用
- `nvidia`: CUDA対応GPUを使う作業向け
  - `BASE_IMAGE=nvidia/cuda:12.6.0-devel-ubuntu22.04` を指定
  - `--gpus all` などの`runArgs`を追加し、GPU計算系の検証に対応
- どちらも同じDockerfileとVS Code設定を共有し、差分を最小化

この分離により、"GPUがなくても全員が同じ構成で開発を継続できる"ことと、"必要時はGPU性能を活用できる"ことを両立しています。

### 2. GitHub ActionsでC++/Pythonライブラリ情報をPagesへ自動デプロイ

- ワークフロー: `.github/workflows/docs-pages.yml`
- `detect_library_changes.py` が `.github/libraries.json` を参照し、対象ライブラリ変更を検出
- `build_docs_site.py` が以下を統合してサイトを生成
  - C++: Doxygen
  - Python: Sphinx
- 生成した静的サイトをGitHub Pagesへ自動デプロイ

これにより、言語混在でもライブラリ情報を1つの公開導線にまとめられます。

### 3. docs編集時にgit hooksで目次更新とCONTRIBUTING.md統合

- フック: `docs/git-hooks/pre-commit`
- `docs/`配下にステージ済み変更がある場合、`docs/script/gen-contribut` を実行
- `gen-contribut` が行う処理
  - `docs/`配下の`_navigation.md`を再帰的に探索し、各ドキュメントの`BEGIN-NAVIGATION`〜`END-NAVIGATION`に目次を再生成
  - `docs/_navigation.md`順に複数ドキュメントを集約し、`CONTRIBUTING.md`を自動生成
  - 生成結果を再ステージ（`docs/**/*.md`, `CONTRIBUTING.md`）

「分散したドキュメント編集」と「最終的な参照導線」を自動で同期できる運用です。

### 4. visualizerを複数制作できるCargo構成

- ルート`Cargo.toml`はworkspaceとして運用
- 深い階層の`ai/visualizer/yew-othello/Cargo.toml`はWeb可視化クレート
- 別系統の`visualizer/Cargo.toml`はデスクトップ試作用クレート

この分割により、可視化を単一実装に固定せず、用途別に複数系統を並行開発しやすくしています。

## 全体図

![画像](/images/repo_readme/overall_view.png)

---

このREADMEは、技術スタックと運用設計の変化に合わせて更新します。
