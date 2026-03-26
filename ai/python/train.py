import argparse
from typing import Tuple

import torch
import torch.nn as nn
from torch.utils.data import DataLoader, random_split
import wandb

from dataset import OthelloBinaryDataset


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


def sign_accuracy(pred: torch.Tensor, target: torch.Tensor) -> float:
    """符号一致率を簡易評価指標として計算する。"""
    pred_sign = torch.sign(pred)
    target_sign = torch.sign(target)
    return float((pred_sign == target_sign).float().mean().item())


def run_epoch(
    model: nn.Module,
    loader: DataLoader,
    criterion: nn.Module,
    device: torch.device,
    optimizer: torch.optim.Optimizer | None,
) -> Tuple[float, float]:
    is_train = optimizer is not None
    model.train(is_train)

    loss_sum = 0.0
    acc_sum = 0.0
    count = 0

    for x, y in loader:
        x = x.to(device)
        y = y.to(device)

        if is_train:
            optimizer.zero_grad()

        pred = model(x)
        loss = criterion(pred, y)

        if is_train:
            loss.backward()
            optimizer.step()

        batch_size = x.size(0)
        loss_sum += float(loss.item()) * batch_size
        acc_sum += sign_accuracy(pred.detach(), y.detach()) * batch_size
        count += batch_size

    mean_loss = loss_sum / max(count, 1)
    mean_acc = acc_sum / max(count, 1)
    return mean_loss, mean_acc


def main() -> None:
    parser = argparse.ArgumentParser(description="Train value network with dataset.bin")
    parser.add_argument("--dataset", type=str, default="ai/data/dataset.bin")
    parser.add_argument("--epochs", type=int, default=3)
    parser.add_argument("--batch-size", type=int, default=256)
    parser.add_argument("--lr", type=float, default=1e-3)
    parser.add_argument("--val-ratio", type=float, default=0.2)
    parser.add_argument("--seed", type=int, default=42)
    parser.add_argument("--no-turn-channel", action="store_true")
    parser.add_argument("--wandb-project", type=str, default="othello-rl")
    parser.add_argument("--wandb-run-name", type=str, default="phase2-mvp")
    parser.add_argument("--save-model", type=str, default="ai/data/value_net_phase2.pth")
    args = parser.parse_args()

    include_turn_channel = not args.no_turn_channel
    dataset = OthelloBinaryDataset(args.dataset, include_turn_channel=include_turn_channel)

    if len(dataset) < 2:
        raise ValueError("dataset must contain at least 2 records")

    val_size = max(1, int(len(dataset) * args.val_ratio))
    train_size = len(dataset) - val_size
    if train_size <= 0:
        train_size = len(dataset) - 1
        val_size = 1

    generator = torch.Generator().manual_seed(args.seed)
    train_ds, val_ds = random_split(dataset, [train_size, val_size], generator=generator)

    train_loader = DataLoader(train_ds, batch_size=args.batch_size, shuffle=True)
    val_loader = DataLoader(val_ds, batch_size=args.batch_size, shuffle=False)

    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    in_channels = 3 if include_turn_channel else 2

    model = ValueNet(in_channels=in_channels).to(device)
    criterion = nn.MSELoss()
    optimizer = torch.optim.Adam(model.parameters(), lr=args.lr)

    wandb.init(
        project=args.wandb_project,
        name=args.wandb_run_name,
        config={
            "dataset": args.dataset,
            "epochs": args.epochs,
            "batch_size": args.batch_size,
            "learning_rate": args.lr,
            "val_ratio": args.val_ratio,
            "include_turn_channel": include_turn_channel,
            "train_size": train_size,
            "val_size": val_size,
            "device": str(device),
        },
    )

    for epoch in range(1, args.epochs + 1):
        train_loss, train_acc = run_epoch(model, train_loader, criterion, device, optimizer)
        val_loss, val_acc = run_epoch(model, val_loader, criterion, device, optimizer=None)

        # Phase 2で必須の学習指標をW&Bへ記録する。
        wandb.log(
            {
                "epoch": epoch,
                "train_loss": train_loss,
                "val_loss": val_loss,
                "train_sign_acc": train_acc,
                "val_sign_acc": val_acc,
                "learning_rate": optimizer.param_groups[0]["lr"],
            }
        )

        print(
            f"epoch={epoch} "
            f"train_loss={train_loss:.6f} val_loss={val_loss:.6f} "
            f"train_sign_acc={train_acc:.4f} val_sign_acc={val_acc:.4f}"
        )

    torch.save(model.state_dict(), args.save_model)
    print(f"saved model: {args.save_model}")

    wandb.finish()


if __name__ == "__main__":
    main()
