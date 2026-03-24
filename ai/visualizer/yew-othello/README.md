# yew-othello

Rust + Yew製 オセロWeb可視化・手動入力アプリ

- Rust (wasm32-unknown-unknown)
- Yewフレームワーク
- trunkでビルド・実行

## 機能
- オセロ盤面表示
- 合法手ハイライト
- クリックで着手（手動入力）
- 手番管理・終了判定

## ビルド・実行方法
1. Rust toolchain, trunk, wasm32ターゲットをインストール
2. trunk serve --open

---

ご要望に応じてAI対戦や棋譜保存なども拡張可能です。
