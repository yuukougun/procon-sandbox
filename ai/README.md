
## Plan: C++×Python オセロ強化学習システム構築（要件管理: aiフォルダ）

### 最終目標

- 強化学習によって「自分（開発者）が対局して勝てない程度」の強さのオセロAIを作ること。
- そのために、人間対AIの対局・検証が容易な可視化ツール（visualizer）も実装する。

自己対戦による強化学習オセロAIのMVPを、C++（ビットボード高速エンジン）＋Python（PyTorch学習）で構築します。

---

### 推奨ディレクトリ構成（ai/配下）

- ai/
  - cpp/ … C++エンジン本体（ビットボード、自己対戦データ生成）
    - include/ … ヘッダファイル
    - src/ … 実装ファイル
    - CMakeLists.txt
    - (pybind11連携時: binding.cpp など)
  - python/ … PyTorch学習・データ処理
    - train.py
    - dataset.py
    - engine.py（C++連携用ラッパー）
  - data/ … 生成した自己対戦データ（バイナリ/CSV等）
  - visualizer/ … オセロ盤面・対局進行の可視化ツール
    - src/ … 実装（例: Rust, Python, C++ いずれも可）
    - README.md … 使い方・仕様
  - README.md … システム要件・設計方針（本ファイル）
- .devcontainer/python-requirements.txt … 開発コンテナ用の必要なpythonパッケージ


---

### C++側ビットボード基本構造

- `BitBoard`クラス
  - 盤面状態（黒/白: uint64_t）
  - 合法手判定（ビット演算）
  - 石の反転処理
  - ゲーム終了判定
  - 盤面状態・手・勝敗のシリアライズ（バイナリ/CSV出力）

- `SelfPlay`クラス
  - ランダム手による自己対戦
  - 1ゲーム分の盤面遷移・手・勝敗を記録
  - データファイル出力

---

### Python側インターフェース設計

#### 連携方式の推奨
- **MVP段階では「バイナリ/CSVファイル経由」**が最もシンプル・デバッグ容易
  - C++: 対局データをファイル出力
  - Python: ファイルをpandas等で読み込み
- 将来的に高速化・柔軟性が必要なら「pybind11による直接呼び出し」も検討

#### Python側構成
- `dataset.py`: C++生成データのパース・PyTorch Dataset化
- `train.py`: モデル定義・学習ループ
- `engine.py`: （将来）pybind11ラッパー or サブプロセス呼び出し

---

### 実装ステップ

#### Phase 1: C++エンジンMVP
1. `BitBoard`クラス実装（合法手判定・反転・終了判定）
2. `SelfPlay`クラスでランダム自己対戦・データ出力（例: CSV形式）

#### Phase 2: Python側MVP
3. `dataset.py`でC++出力データを読み込み、PyTorch Dataset化
4. `train.py`で簡易なNNモデル・学習ループ（データが読めることの検証）

#### Phase 3: インターフェース拡張
5. pybind11によるC++エンジンのPythonラッパー（必要に応じて）

---

### 主要ファイル

- ai/cpp/include/BitBoard.hpp … ビットボードクラス定義
- ai/cpp/include/SelfPlay.hpp … 自己対戦クラス定義
- ai/cpp/src/ … 実装
- ai/python/dataset.py … データ読み込み
- ai/python/train.py … 学習スクリプト
- ai/requirements.md … 本要件・設計方針

---

### 検証方法

- C++: ランダム自己対戦データが正しく出力されるか（テスト用main, ユニットテスト）
- Python: データが正しく読み込めるか（pandas, PyTorch Datasetでshape/内容確認）
- （将来）C++⇔Python連携の速度・正確性

---

### 決定事項・スコープ

---

### Visualizer（対局可視化ツール）実装要件

- 人間 vs AI、AI vs AI の対局をGUIまたはCUIで可視化できること
- 盤面表示（黒石・白石・合法手のハイライト）
- 着手入力（人間が手を選択できるUI、またはコマンド）
- 対局ログの保存・再生（リプレイ）
- C++エンジン・Python学習器との連携（盤面状態・手の受け渡し）
- 最低限、CLIで動作するCUI版を先に実装し、余裕があればGUI（例: Rust egui, Python tkinter, Web等）も検討

- MVPは「ファイル経由連携」で実装
- 外部棋譜は使わず、自己対戦のみ
- まずはランダム手によるデータ生成・読み込みまで
- NN設計・強化学習ロジックはMVP後に拡張

