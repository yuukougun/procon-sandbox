"""Phase 4: 自己対戦→学習→モデル更新→対戦評価の自動ループ。"""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
from pathlib import Path
from typing import List

from engine import generate_dataset_beam, play_duel_once


def run(cmd: List[str]) -> str:
    out = subprocess.check_output(cmd, text=True)
    return out.strip()


def concat_replay_files(files: List[Path], out_path: Path) -> None:
    out_path.parent.mkdir(parents=True, exist_ok=True)
    with out_path.open("wb") as out_f:
        for f in files:
            with f.open("rb") as in_f:
                shutil.copyfileobj(in_f, out_f)


def main() -> None:
    parser = argparse.ArgumentParser(description="Phase4 auto loop")
    parser.add_argument("--base-checkpoint", required=True)
    parser.add_argument("--cycles", type=int, default=3)
    parser.add_argument("--num-games", type=int, default=100)
    parser.add_argument("--duel-games", type=int, default=20)
    parser.add_argument("--replay-window", type=int, default=3)
    parser.add_argument("--train-epochs", type=int, default=5)
    parser.add_argument("--batch-size", type=int, default=512)
    parser.add_argument("--lr", type=float, default=1e-3)
    args = parser.parse_args()

    replay_dir = Path("ai/data/replay")
    replay_dir.mkdir(parents=True, exist_ok=True)
    history_path = Path("ai/data/phase4_loop_history.jsonl")

    current_ckpt = Path(args.base_checkpoint)
    current_model_id = current_ckpt.stem

    for cycle in range(1, args.cycles + 1):
        cycle_id = f"cycle-{cycle:03d}"
        cycle_dataset = replay_dir / f"{cycle_id}.bin"

        # 1) 現モデルで自己対戦データを生成（pybind）
        for i in range(args.num_games):
            generate_dataset_beam(
                mode="guided",
                dataset_path=str(cycle_dataset),
                model_path=str(current_ckpt),
                model_side="black",
                beam_width=32,
                beam_top_k=2,
                tie_break="random",
                log_interval=1,
                seed=42 + cycle * 1000 + i,
            )

        # 2) リプレイバッファ（直近N世代）を結合して学習用データを作る
        all_cycle_files = sorted(Path("ai/data/replay").glob("cycle-*.bin"))
        selected = all_cycle_files[-args.replay_window :]
        train_dataset = Path("ai/data/dataset_train.bin")
        concat_replay_files(selected, train_dataset)

        # 3) 再開学習で新モデルを作成
        new_model_id = f"value-phase4-{cycle_id}"
        run(
            [
                sys.executable,
                "ai/python/train.py",
                "--dataset",
                str(train_dataset),
                "--epochs",
                str(args.train_epochs),
                "--batch-size",
                str(args.batch_size),
                "--lr",
                str(args.lr),
                "--resume-model",
                str(current_ckpt),
                "--parent-model-id",
                current_model_id,
                "--model-id",
                new_model_id,
                "--wandb-project",
                "othello-rl",
                "--wandb-group",
                "phase4-autoloop",
                "--wandb-run-name",
                new_model_id,
                "--baseline-run-id",
                current_model_id,
            ]
        )

        new_ckpt = Path(f"ai/data/models/{new_model_id}.ckpt")

        # 4) 旧新モデルの対戦評価（黒=新 / 白=旧, pybind）
        black_wins = 0
        white_wins = 0
        draws = 0
        for i in range(args.duel_games):
            result = play_duel_once(
                model_black=str(new_ckpt),
                model_white=str(current_ckpt),
                tie_break="random",
                seed=84 + cycle * 2000 + i,
            )
            if result > 0:
                black_wins += 1
            elif result < 0:
                white_wins += 1
            else:
                draws += 1

        duel_info = {
            "games": args.duel_games,
            "black_wins": black_wins,
            "white_wins": white_wins,
            "draws": draws,
        }
        adopted = black_wins >= white_wins

        prev_model_id = current_model_id
        if adopted:
            current_ckpt = new_ckpt
            current_model_id = new_model_id

        history_entry = {
            "cycle": cycle,
            "cycle_id": cycle_id,
            "parent_model_id": prev_model_id,
            "candidate_model_id": new_model_id,
            "adopted": adopted,
            "duel": duel_info,
            "replay_files": [str(p) for p in selected],
            "train_dataset": str(train_dataset),
            "current_checkpoint": str(current_ckpt),
        }
        with history_path.open("a", encoding="utf-8") as f:
            f.write(json.dumps(history_entry, ensure_ascii=False) + "\n")

        print(json.dumps(history_entry, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
