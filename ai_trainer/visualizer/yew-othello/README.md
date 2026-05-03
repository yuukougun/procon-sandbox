# yew-othello

Rust + Yew製 オセロWeb可視化・手動入力アプリ

- Rust (wasm32-unknown-unknown)
- Yewフレームワーク
- trunkでビルド・実行

## 人間(黒) vs 学習済みモデル(白)

このモードでは、YewアプリはHTTP経由で Python 推論サーバーへ着手を問い合わせます。

### 1. pybind拡張をビルド

```
source /home/dev/.venv/bin/activate
python ai/python/build_cpp_engine.py build_ext
```

### 2. 推論サーバー起動

```
source /home/dev/.venv/bin/activate
python ai/python/model_move_server.py --host 127.0.0.1 --port 8000
```

### 3. Yewアプリ起動

```
cd ai/visualizer/yew-othello
trunk serve --open
```

画面上で以下を設定します。
- AI API: `http://127.0.0.1:8000`
- Model: `ai/data/models/<your_model>.ckpt` もしくは `.pth`
- 「白をAIにする」をON

これで、黒が人間入力・白が学習済みモデルで進行します。

## 機能
- オセロ盤面表示
- 合法手ハイライト
- クリックで着手（手動入力）
- 手番管理・終了判定
- 人間(黒) vs AI(白) 対戦（HTTP推論連携）

## ビルド・実行方法
1. Rust toolchain, trunk, wasm32ターゲットをインストール
2. trunk serve --open

---

ご要望に応じてAI対戦や棋譜保存なども拡張可能です。
