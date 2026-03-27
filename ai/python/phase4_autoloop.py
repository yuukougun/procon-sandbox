"""Phase 4: 自己対戦→学習→モデル更新→対戦評価の自動ループ。"""

from __future__ import annotations

import argparse
import glob
import json
import os
import shutil
import subprocess
from pathlib import Path
from typing import List


def run(cmd: List[str]) -> str:
    out = subprocess.check_output(cmd, text=True)
    return out.strip()


def compile_cpp() -> None:
    run(
        [
            "g++",
            "-std=c++17",
            "-O2",
            "ai/cpp/src/BitBoard.cpp",
            "ai/cpp/src/SelfPlay.cpp",
            "ai/cpp/src/main.cpp",
            "-Iai/cpp/include",
            "-o",
            "ai/cpp/bin/selfplay",
        ]
    )


def concat_replay_files(files: List[Path], out_path: Path) -> None:
    out_path.parent.mkdir(parents=True, exist_ok=True)
    with out_path.open("wb") as out_f:
        for f in files:
            with f.open("rb") as in_f:
                shutil.copyfileobj(in_f, out_f)


def parse_duel_result(text: str) -> dict:
    # 期待形式: games=... black_wins=... white_wins=... draws=...
    parts = text.split()
    info = {}
    for p in parts:
        if "=" in p:
            k, v = p.split("=", 1)
            if v.isdigit():
                info[k] = int(v)
    return info


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

    compile_cpp()

    replay_dir = Path("ai/data/replay")
    replay_dir.mkdir(parents=True, exist_ok=True)
    history_path = Path("ai/data/phase4_loop_history.jsonl")

    current_ckpt = Path(args.base_checkpoint)
    current_model_id = current_ckpt.stem

    for cycle in range(1, args.cycles + 1):
        cycle_id = f"cycle-{cycle:03d}"
        cycle_dataset = replay_dir / f"{cycle_id}.bin"

        # 1) 現モデルで自己対戦データを生成
        run(
            [
                "ai/cpp/bin/selfplay",
                "--mode",
                "guided",
                "--model",
                str(current_ckpt),
                "--inference-script",
                "ai/python/value_inference_wrapper.py",
                "--dataset",
                str(cycle_dataset),
                "--num-games",
                str(args.num_games),
                "--tie-break",
                "random",
            ]
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
                "python",
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

        # 4) 旧新モデルの対戦評価（黒=新 / 白=旧）
        duel_text = run(
            [
                "ai/cpp/bin/selfplay",
                "--mode",
                "duel",
                "--model-black",
                str(new_ckpt),
                "--model-white",
                str(current_ckpt),
                "--inference-script",
                "ai/python/value_inference_wrapper.py",
                "--num-games",
                str(args.duel_games),
                "--tie-break",
                "random",
            ]
        )
        duel_info = parse_duel_result(duel_text)

        black_wins = duel_info.get("black_wins", 0)
        white_wins = duel_info.get("white_wins", 0)
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
