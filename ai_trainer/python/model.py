"""Value only モデル定義とロードユーティリティ。"""

from __future__ import annotations

import torch
import torch.nn as nn


class ValueNet(nn.Module):
    """盤面から勝敗価値（-1~1）を予測する小型CNN。"""

    def __init__(self, in_channels: int) -> None:
        super().__init__()
        self.net = nn.Sequential(
            nn.Conv2d(in_channels, 32, kernel_size=3, padding=1),
            nn.ReLU(),
            nn.Conv2d(32, 64, kernel_size=3, padding=1),
            nn.ReLU(),
            nn.Flatten(),
            nn.Linear(64 * 8 * 8, 128),
            nn.ReLU(),
            nn.Linear(128, 1),
        )

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        return self.net(x)


def load_value_model(
    model_path: str,
    in_channels: int,
    device: torch.device,
) -> ValueNet:
    """pth / ckpt の両形式を読み込んでモデルを返す。"""
    model = ValueNet(in_channels=in_channels).to(device)
    obj = torch.load(model_path, map_location=device)

    if isinstance(obj, dict) and "model_state_dict" in obj:
        model.load_state_dict(obj["model_state_dict"])
    else:
        model.load_state_dict(obj)

    model.eval()
    return model
