"""C++ から呼び出す Value 推論ラッパー。

入力ファイル形式:
- 1行目: サンプル数 N
- 2行目以降: `black white black_to_move`

出力ファイル形式:
- N行、各行に推論値(float)
"""

from __future__ import annotations

import argparse
from typing import List, Tuple

import numpy as np
import torch

from dataset import bitboard_to_planes
from model import load_value_model


def read_input(path: str) -> List[Tuple[int, int, int]]:
    with open(path, "r", encoding="utf-8") as f:
        lines = [line.strip() for line in f if line.strip()]

    n = int(lines[0])
    rows = []
    for line in lines[1:1 + n]:
        b, w, t = line.split()
        rows.append((int(b), int(w), int(t)))
    return rows


def main() -> None:
    parser = argparse.ArgumentParser(description="Value inference wrapper")
    parser.add_argument("--model", required=True)
    parser.add_argument("--input-file", required=True)
    parser.add_argument("--output-file", required=True)
    parser.add_argument("--device", default="cpu", choices=["cpu", "cuda"])
    parser.add_argument("--no-turn-channel", action="store_true")
    args = parser.parse_args()

    include_turn_channel = not args.no_turn_channel
    in_channels = 3 if include_turn_channel else 2

    device = torch.device(args.device if args.device == "cpu" or torch.cuda.is_available() else "cpu")
    model = load_value_model(args.model, in_channels=in_channels, device=device)

    rows = read_input(args.input_file)
    planes = [
        bitboard_to_planes(black, white, turn, include_turn_channel=include_turn_channel)
        for black, white, turn in rows
    ]

    x = torch.from_numpy(np.stack(planes, axis=0)).to(device)
    with torch.no_grad():
        y = model(x).squeeze(1).detach().cpu().numpy()

    with open(args.output_file, "w", encoding="utf-8") as f:
        for v in y:
            f.write(f"{float(v):.10f}\n")


if __name__ == "__main__":
    main()
