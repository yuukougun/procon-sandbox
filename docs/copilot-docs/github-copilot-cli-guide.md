# GitHub Copilot CLI 完全ガイド（基本・応用・全コマンド・全ショートカット）

このドキュメントは、GitHub Copilot CLI の公式ヘルプ出力と公式ドキュメントをもとに、実運用で必要な情報を 1 つにまとめたものです。

## 1. 基本的な使い方

### 1-1. インストール
- macOS / Linux: `curl -fsSL https://gh.io/copilot-install | bash`
- Homebrew: `brew install copilot-cli`
- Windows (WinGet): `winget install GitHub.Copilot`
- npm: `npm install -g @github/copilot`

### 1-2. 起動と初期設定
1. プロジェクトディレクトリで `copilot` を実行
2. 未ログイン時は `/login` を実行して認証
3. 必要に応じて `/model` でモデルを切り替え

PAT で認証する場合は `GH_TOKEN` または `GITHUB_TOKEN` を設定します（Copilot Requests 権限付きトークン）。

### 1-3. 最初に覚える操作
- 自由文で依頼: そのまま自然言語で入力
- ファイル参照: `@` でファイル mention
- Issue / PR 参照: `#` で mention
- シェル実行: `!` でコマンド実行
- ヘルプ: `/help`

## 2. 応用的な使い方

### 2-1. 実装効率を上げる運用
- `/plan`: 実装前に計画を作る
- `/review`: 変更差分に対してコードレビューを実行
- `/diff`: 現在ディレクトリの変更を確認
- `/pr`: 現在ブランチの PR 操作
- `/tasks`, `/sidekicks`: サブエージェント/タスクの進行管理

### 2-2. 大規模・自動化寄りの運用
- `/delegate`: セッションを GitHub に送って PR 作成を委譲
- `/fleet`: 並列サブエージェント実行を有効化
- `/research`: GitHub 検索と Web を使う深掘り調査
- `/mcp`, `/skills`, `/plugin`: 拡張機能・外部連携を管理

### 2-3. LSP 連携（コード理解を強化）
LSP サーバーを別途インストールし、以下で設定します。
- ユーザー単位: `~/.copilot/lsp-config.json`
- リポジトリ単位: `.github/lsp.json`

確認は `/lsp` で行えます。

### 2-4. モード・実験機能
- `Shift+Tab`: モード切替（Autopilot を含む）
- `copilot --experimental` または `/experimental`: 実験機能を有効化
- `copilot --banner`: 起動バナー表示

## 3. 全コマンド解説（`/help` ベース）

## 3-1. グローバル入力
- `/help`: 全ヘルプ表示
- `/`: コマンド入力開始
- `@`: ファイル mention
- `#`: Issue/PR mention
- `!`: シェルコマンド実行

## 3-2. Agent Environment
- `/init`: リポジトリ用 Copilot instructions を初期化
- `/agent`: 利用可能エージェントを選択
- `/skills`: スキル管理
- `/mcp`: MCP サーバー設定管理
- `/plugin`: プラグイン/マーケットプレイス管理

## 3-3. Agents / Subagents
- `/model`: 使用モデルを選択
- `/delegate`: セッションを GitHub 側に委譲し PR 作成
- `/fleet`: 並列サブエージェント実行を有効化
- `/tasks`: サブエージェント/シェルタスク管理

## 3-4. Code
- `/ide`: IDE ワークスペース接続
- `/diff`: 現在ディレクトリの差分確認
- `/pr`: 現在ブランチの PR 操作
- `/review`: コードレビューエージェント実行
- `/lsp`: LSP 設定管理
- `/terminal-setup`: 複数行入力（Shift+Enter）向け端末設定

## 3-5. Permissions
- `/allow-all`: ツール/パス/URL 許可を一括有効化
- `/add-dir`: ファイルアクセス許可ディレクトリ追加
- `/list-dirs`: 許可ディレクトリ一覧表示
- `/cwd`: 作業ディレクトリ変更/表示
- `/reset-allowed-tools`: 許可ツール一覧をリセット

## 3-6. Session
- `/resume`: 別セッションに切替
- `/rename`: セッション名変更（自動生成可）
- `/context`: コンテキスト使用量可視化
- `/usage`: 利用メトリクス表示
- `/session`: セッション管理
- `/compact`: 履歴を要約してコンテキスト節約
- `/share`: セッション/調査結果を Markdown/HTML/Gist へ共有
- `/remote`: GitHub Web/Mobile からのリモート制御管理
- `/copy`: 直前レスポンスをクリップボードへコピー
- `/rewind`: 直前ターンに巻き戻し＋ファイル変更取り消し

## 3-7. Help
- `/help`: インタラクティブコマンドのヘルプ
- `/changelog`: CLI バージョン変更履歴表示（`summarize` で要約）
- `/feedback`: フィードバック送信
- `/theme`: カラーモード表示/設定
- `/statusline`: ステータスライン項目設定
- `/footer`: ステータスライン項目設定
- `/update`: CLI を最新へ更新
- `/version`: バージョン表示と更新確認
- `/experimental`: 実験機能表示、実験モード ON/OFF
- `/clear`: セッション破棄して新規開始
- `/instructions`: カスタム instructions ファイル表示/切替
- `/streamer-mode`: 配信向け表示抑制モード切替

## 3-8. Other commands
- `/ask`: 履歴に残さないサイド質問
- `/chronicle`: セッション履歴/インサイト機能
- `/env`: 読み込まれた環境情報表示（instructions/MCP/skills/agents/plugins/LSP/extensions）
- `/exit`: CLI 終了
- `/keep-alive`: スリープ抑止モード管理
- `/login`: Copilot ログイン
- `/logout`: OAuth セッションからログアウト
- `/new`: 新しい会話を開始
- `/plan`: 実装前に計画を作成
- `/research`: GitHub 検索と Web 情報による深掘り調査
- `/restart`: 現在セッションを保持して CLI 再起動
- `/search`: 会話タイムライン検索
- `/sidekicks`: 稼働中 sidekick agent 一覧
- `/undo`: 直前ターンに巻き戻し＋ファイル変更取り消し
- `/user`: GitHub ユーザーリスト管理

## 4. 全ショートカットキー解説

## 4-1. グローバル
- `Shift+Tab`: モード切替
- `Ctrl+S`: コマンド実行（入力維持）
- `Ctrl+O / Ctrl+E`: タイムライン全展開
- `Ctrl+C`: キャンセル
- `Ctrl+C` を 2 回: 終了
- `Esc`: キャンセル
- `Ctrl+D`: シャットダウン
- `Ctrl+L`: 画面クリア
- `Ctrl+T`: 推論表示トグル
- `Ctrl+X` → `B`: 現在タスクをバックグラウンドへ移動
- `Ctrl+X` → `O`: 直近リンクを開く

## 4-2. 入力編集
- `Ctrl+A`: 行頭へ移動
- `Ctrl+E`: 行末へ移動
- `Ctrl+H`: 直前 1 文字削除
- `Ctrl+W`: 直前 1 単語削除
- `Ctrl+U`: カーソル位置から行頭まで削除
- `Ctrl+K`: カーソル位置から行末まで削除
- `Meta+← / Meta+→`: 単語単位で移動
- `Ctrl+G`: `$EDITOR` でプロンプト編集

## 5. 実運用のコツ

1. まず `/help`, `/model`, `/diff`, `/review` を使える状態にする。  
2. 大きな作業は `/plan` → 実装 → `/review` の順で進める。  
3. 調査は `/research`、並列処理は `/fleet` と `/tasks` を活用する。  
4. LSP を設定し、`@` mention と `/lsp` を併用してコード理解精度を上げる。  

## 6. 補足（instructions 読み込み先）

Copilot は次の instructions を参照します。
- `CLAUDE.md`
- `GEMINI.md`
- `AGENTS.md`（git root / cwd）
- `.github/instructions/**/*.instructions.md`（git root / cwd）
- `.github/copilot-instructions.md`
- `$HOME/.copilot/copilot-instructions.md`
- `COPILOT_CUSTOM_INSTRUCTIONS_DIRS`（環境変数）

---

公式ドキュメント:
- https://docs.github.com/copilot/how-tos/use-copilot-agents/use-copilot-cli
- https://docs.github.com/copilot/concepts/agents/about-copilot-cli
