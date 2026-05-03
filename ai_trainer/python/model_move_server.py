"""Human-vs-model move server for yew-othello.

Endpoint:
- GET /ai-move?black=<u64>&white=<u64>&black_to_move=<0|1>&model_path=<path>
  -> returns move index as plain text (-1 for pass)
"""

from __future__ import annotations

import argparse
import sys
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from typing import Dict, Tuple
from urllib.parse import parse_qs, urlparse

import numpy as np
import torch

def _append_cpp_engine_paths() -> None:
    repo_root = Path(__file__).resolve().parents[2]
    candidates = [
        repo_root,
        repo_root / "ai/python/build",
    ]
    candidates.extend((repo_root / "ai/python/build").glob("lib.*"))

    for p in candidates:
        if p.exists():
            s = str(p)
            if s not in sys.path:
                sys.path.insert(0, s)


try:
    import othello_cpp_engine as cpp
except ModuleNotFoundError:
    _append_cpp_engine_paths()
    import othello_cpp_engine as cpp

try:
    from dataset import bitboard_to_planes
    from model import load_value_model
except ModuleNotFoundError:
    from ai.python.dataset import bitboard_to_planes
    from ai.python.model import load_value_model

ModelCacheValue = Tuple[torch.nn.Module, torch.device, bool]
_MODEL_CACHE: Dict[str, ModelCacheValue] = {}


def resolve_model_file(model_path: str) -> Path:
    p = Path(model_path)
    if not p.is_absolute():
        p = (Path(__file__).resolve().parents[2] / p).resolve()

    if p.is_file():
        return p

    if p.is_dir():
        candidates = sorted(
            [
                *p.glob("*.ckpt"),
                *p.glob("*.pth"),
            ],
            key=lambda x: x.stat().st_mtime,
            reverse=True,
        )
        if not candidates:
            raise FileNotFoundError(f"no model file found in directory: {p}")
        return candidates[0]

    raise FileNotFoundError(f"model_path not found: {p}")


def load_cached_model(model_path: str, no_turn_channel: bool, device_name: str) -> ModelCacheValue:
    model_file = resolve_model_file(model_path)
    key = f"{model_file}|{int(no_turn_channel)}|{device_name}"
    cached = _MODEL_CACHE.get(key)
    if cached is not None:
        return cached

    include_turn_channel = not no_turn_channel
    in_channels = 3 if include_turn_channel else 2
    requested = torch.device(device_name)
    if requested.type == "cuda" and not torch.cuda.is_available():
        requested = torch.device("cpu")

    model = load_value_model(str(model_file), in_channels=in_channels, device=requested)
    value = (model, requested, include_turn_channel)
    _MODEL_CACHE[key] = value
    return value


def evaluate_moves(
    black: int,
    white: int,
    black_to_move: int,
    model_path: str,
    no_turn_channel: bool,
    device_name: str,
) -> int:
    board = cpp.BitBoard(black, white, bool(black_to_move))
    legal = board.get_legal_move_indices()
    if not legal:
        return -1

    model, device, include_turn_channel = load_cached_model(model_path, no_turn_channel, device_name)

    planes = []
    for mv in legal:
        child = cpp.BitBoard(black, white, bool(black_to_move))
        child.do_move(mv)
        next_turn = 1 if child.black_to_move else 0
        planes.append(bitboard_to_planes(child.black, child.white, next_turn, include_turn_channel))

    x = torch.from_numpy(np.stack(planes, axis=0)).to(device)
    with torch.no_grad():
        y = model(x).squeeze(1).detach().cpu().numpy()

    # 学習値は黒視点なので、黒手番は最大化・白手番は最小化する。
    if int(black_to_move) == 1:
        best_idx = int(np.argmax(y))
    else:
        best_idx = int(np.argmin(y))

    return int(legal[best_idx])


class MoveHandler(BaseHTTPRequestHandler):
    server_version = "OthelloMoveServer/0.1"

    def end_headers(self) -> None:
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        super().end_headers()

    def do_OPTIONS(self) -> None:
        self.send_response(204)
        self.end_headers()

    def do_GET(self) -> None:
        parsed = urlparse(self.path)

        if parsed.path == "/health":
            self.send_response(200)
            self.send_header("Content-Type", "text/plain; charset=utf-8")
            self.end_headers()
            self.wfile.write(b"ok")
            return

        if parsed.path != "/ai-move":
            self.send_response(404)
            self.send_header("Content-Type", "text/plain; charset=utf-8")
            self.end_headers()
            self.wfile.write(b"not found")
            return

        query = parse_qs(parsed.query)

        try:
            black = int(query.get("black", [""])[0])
            white = int(query.get("white", [""])[0])
            black_to_move = int(query.get("black_to_move", [""])[0])
            model_path = query.get("model_path", [""])[0]

            if black_to_move not in (0, 1):
                raise ValueError("black_to_move must be 0 or 1")
            if not model_path:
                raise ValueError("model_path is required")

            no_turn_channel = query.get("no_turn_channel", ["0"])[0] == "1"
            device_name = query.get("device", ["cpu"])[0]

            mv = evaluate_moves(
                black=black,
                white=white,
                black_to_move=black_to_move,
                model_path=model_path,
                no_turn_channel=no_turn_channel,
                device_name=device_name,
            )

            body = str(mv).encode("utf-8")
            self.send_response(200)
            self.send_header("Content-Type", "text/plain; charset=utf-8")
            self.send_header("Content-Length", str(len(body)))
            self.end_headers()
            self.wfile.write(body)
        except Exception as e:  # pragma: no cover
            body = str(e).encode("utf-8")
            self.send_response(400)
            self.send_header("Content-Type", "text/plain; charset=utf-8")
            self.send_header("Content-Length", str(len(body)))
            self.end_headers()
            self.wfile.write(body)


def main() -> None:
    parser = argparse.ArgumentParser(description="Move inference server for yew-othello")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=8000)
    args = parser.parse_args()

    httpd = ThreadingHTTPServer((args.host, args.port), MoveHandler)
    print(f"listening on http://{args.host}:{args.port}")
    httpd.serve_forever()


if __name__ == "__main__":
    main()
