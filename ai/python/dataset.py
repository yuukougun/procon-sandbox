import os
from typing import Tuple

import numpy as np
import torch
from torch.utils.data import Dataset

# 1局面 = 19バイトの固定長レコード
# uint64 black, uint64 white, int8 black_to_move, int8 move, int8 result
TRAINING_DTYPE = np.dtype(
    [
        ("black", "<u8"),
        ("white", "<u8"),
        ("black_to_move", "i1"),
        ("move", "i1"),
        ("result", "i1"),
    ],
    align=False,
)


def bitboard_to_planes(
    black: int,
    white: int,
    black_to_move: int,
    include_turn_channel: bool,
) -> np.ndarray:
    """ビットボードを(2 or 3, 8, 8)テンソルへ変換する。"""
    channels = 3 if include_turn_channel else 2
    board = np.zeros((channels, 8, 8), dtype=np.float32)

    # bit indexは C++ 側の pos = y * 8 + x に対応。
    for pos in range(64):
        y = pos // 8
        x = pos % 8
        if (black >> pos) & 1:
            board[0, y, x] = 1.0
        elif (white >> pos) & 1:
            board[1, y, x] = 1.0

    if include_turn_channel:
        board[2, :, :] = float(black_to_move)

    return board


class OthelloBinaryDataset(Dataset):
    """dataset.bin を読み込み、盤面テンソルと勝敗ラベルを返す Dataset。"""

    def __init__(
        self,
        file_path: str,
        include_turn_channel: bool = True,
    ) -> None:
        self.file_path = file_path
        self.include_turn_channel = include_turn_channel

        if not os.path.exists(file_path):
            raise FileNotFoundError(f"dataset file not found: {file_path}")

        file_size = os.path.getsize(file_path)
        record_size = TRAINING_DTYPE.itemsize
        if file_size == 0:
            raise ValueError(f"dataset file is empty: {file_path}")
        if file_size % record_size != 0:
            raise ValueError(
                f"invalid dataset size: {file_size} bytes (must be multiple of {record_size})"
            )

        self.records = np.fromfile(file_path, dtype=TRAINING_DTYPE)

    def __len__(self) -> int:
        return len(self.records)

    def __getitem__(self, index: int) -> Tuple[torch.Tensor, torch.Tensor]:
        rec = self.records[index]
        x = bitboard_to_planes(
            int(rec["black"]),
            int(rec["white"]),
            int(rec["black_to_move"]),
            self.include_turn_channel,
        )

        # result は {-1, 0, 1} の価値ラベルとして回帰する。
        y = np.array([float(rec["result"])], dtype=np.float32)

        return torch.from_numpy(x), torch.from_numpy(y)


def quick_check(file_path: str, include_turn_channel: bool = True) -> None:
    """データ読み込みの最小確認を行う。"""
    ds = OthelloBinaryDataset(file_path, include_turn_channel=include_turn_channel)
    x0, y0 = ds[0]

    results = ds.records["result"]
    unique, counts = np.unique(results, return_counts=True)
    dist = {int(k): int(v) for k, v in zip(unique, counts)}

    print(f"records={len(ds)}")
    print(f"x_shape={tuple(x0.shape)}, y_shape={tuple(y0.shape)}")
    print(f"result_distribution={dist}")


if __name__ == "__main__":
    default_path = "ai/data/dataset.bin"
    quick_check(default_path, include_turn_channel=True)
