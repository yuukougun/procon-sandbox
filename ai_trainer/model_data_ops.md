# モデル重み再利用とデータ管理ルール

## 1. 目的
- Value only 学習を継続しながら、モデル再利用とデータ由来の追跡性を確保する。
- wandb とローカルJSONの両方で、実験比較を再現可能にする。

## 2. 保存名ルール
- `model_id`: `value-YYYYMMDD_HHMMSS` を標準とする。
- 重み保存先: `ai/data/models/<model_id>.pth`
- チェックポイント保存先: `ai/data/models/<model_id>.ckpt`
- メトリクス保存先: `ai/data/metrics_<experiment_id>.json`

## 3. 再開手順（重み再利用）
1. 前モデルのチェックポイントを指定して再開する。
2. 実行例:
   - `python ai/python/train.py --resume-model ai/data/models/<old_model_id>.ckpt --parent-model-id <old_model_id> --model-id <new_model_id>`
3. `--parent-model-id` は継承元を必ず記録する。

## 4. 比較基準runルール
- 比較対象の run ID を `--baseline-run-id` で指定する。
- wandb では以下を固定比較軸とする。
  - `best_val_loss`
  - `best_val_sign_acc`
  - `dataset_records`
  - `stability_ok`

## 5. モデルIDとデータ生成元の対応付け
- 学習終了時に `ai/data/model_registry.jsonl` へ1行追記する。
- 各行に最低限記録する項目:
  - `model_id`
  - `parent_model_id`
  - `experiment_id`
  - `dataset`（データファイルパス）
  - `dataset_records`
  - `best_val_loss`
  - `best_val_sign_acc`
  - `final_model_path`
  - `final_checkpoint_path`

## 6. 3チャネル安定性確認ルール
- 標準入力は `black/white/turn` の3チャネル。
- 学習実行で以下を満たした場合を安定とみなす。
  - 1エポック以上完走
  - `stability_ok = true`
  - `best_val_loss` が有限値

## 7. 補足
- 古いモデル由来データを使う場合も `parent_model_id` と `baseline_run_id` を必ず残す。
- 対局データの採用比率（新規/旧世代）はPhase 4以降で明示的に管理する。
