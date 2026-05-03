"""dataset.bin を PyTorch で扱うためのデータローダ。

このファイルの役割:
1. C++ 側が出力した固定長バイナリを安全に読む
2. ビットボードをニューラルネット入力テンソルへ変換する
3. 学習器が受け取れる (x, y) 形式を返す

学習の観点で重要なのは「データ形式を厳密に揃える」ことです。
ここが崩れると、モデル設計が正しくても学習が進みません。
"""

import os
from typing import Tuple

import numpy as np
import torch
from torch.utils.data import Dataset

# 1局面 = 19バイトの固定長レコード
#   uint64 black         : 黒石配置のビット列
#   uint64 white         : 白石配置のビット列
#   int8   black_to_move : 手番 (1:黒, 0:白)
#   int8   move          : 着手位置 (0-63, パスは-1)
#   int8   result        : 最終勝敗 (1, 0, -1)
#
# <u8 を使って little-endian を明示し、C++ 側との解釈差を防ぐ。
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
    """ビットボードを (C, H, W) = (2 or 3, 8, 8) テンソルへ変換する。

    チャネル設計:
    - channel 0: 黒石 (0 or 1)
    - channel 1: 白石 (0 or 1)
    - channel 2: 手番 (盤面全体に 0 or 1 を敷き詰める, 任意)

    直感的には、盤面を「画像」としてCNNに入力する前処理です。
    """
    channels = 3 if include_turn_channel else 2
    board = np.zeros((channels, 8, 8), dtype=np.float32)

    # bit index は C++ 側の pos = y * 8 + x と同じ。
    # ここを揃えることで、学習時に盤面が上下左右で崩れる事故を防ぐ。
    for pos in range(64):
        y = pos // 8
        x = pos % 8
        if (black >> pos) & 1:
            board[0, y, x] = 1.0
        elif (white >> pos) & 1:
            board[1, y, x] = 1.0

    if include_turn_channel:
        # 手番を盤面全体へ定数チャネルとして与える。
        # これでモデルは「同じ石配置でも手番で価値が変わる」ことを学習できる。
        board[2, :, :] = float(black_to_move)

    return board


class OthelloBinaryDataset(Dataset):
    """dataset.bin を読み込み、盤面テンソルと勝敗ラベルを返す Dataset。

    返す値:
    - x: shape = (2 or 3, 8, 8), dtype=float32
    - y: shape = (1,), 値域は {-1, 0, 1}
    """

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

        # 学習前にファイル整合性を検証し、壊れたデータを早期検知する。
        if file_size == 0:
            raise ValueError(f"dataset file is empty: {file_path}")
        if file_size % record_size != 0:
            raise ValueError(
                f"invalid dataset size: {file_size} bytes (must be multiple of {record_size})"
            )

        # np.fromfile は高速だが、dtypeが仕様とズレると全データが壊れる。
        # そのため dtype は上で固定し、ここではそのまま読むだけにする。
        self.records = np.fromfile(file_path, dtype=TRAINING_DTYPE)

    def __len__(self) -> int:
        return len(self.records)

    def __getitem__(self, index: int) -> Tuple[torch.Tensor, torch.Tensor]:
        # 構造化配列の 1 レコードを取り出してテンソルへ変換する。
        rec = self.records[index]
        x = bitboard_to_planes(
            int(rec["black"]),
            int(rec["white"]),
            int(rec["black_to_move"]),
            self.include_turn_channel,
        )

        # result を「局面価値」と見なして回帰する。
        # 例: +1に近い予測ほど黒有利、-1に近い予測ほど白有利。
        y = np.array([float(rec["result"])], dtype=np.float32)

        return torch.from_numpy(x), torch.from_numpy(y)


def quick_check(file_path: str, include_turn_channel: bool = True) -> None:
    """データ読み込みの最小確認を行う。

    確認ポイント:
    - 総レコード数
    - 1サンプルのテンソルshape
    - 勝敗ラベル分布（偏り確認）
    """
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
