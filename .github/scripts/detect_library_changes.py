#!/usr/bin/env python3
"""Detect whether docs workflow should run based on configured library paths."""

from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
from pathlib import Path


def write_output(output_path: Path, key: str, value: str) -> None:
    with output_path.open("a", encoding="utf-8") as fh:
        fh.write(f"{key}={value}\n")


def run_git_diff(before_sha: str, current_sha: str) -> list[str]:
    null_sha = "0" * 40
    if not before_sha or before_sha == null_sha:
        result = subprocess.check_output(["git", "ls-files"], text=True)
        return result.splitlines()

    result = subprocess.check_output(
        ["git", "diff", "--name-only", before_sha, current_sha],
        text=True,
    )
    return result.splitlines()


def build_watched_prefixes(config_path: Path) -> set[str]:
    config = json.loads(config_path.read_text(encoding="utf-8"))
    watched_prefixes = {str(config_path.as_posix()).strip("/")}

    for library in config.get("libraries", []):
        for source_path in library.get("source_paths", []):
            watched_prefixes.add(str(Path(source_path).as_posix().strip("/")))

    return watched_prefixes


def should_run_for_changes(changed_files: list[str], watched_prefixes: set[str]) -> bool:
    for file_path in changed_files:
        normalized = file_path.strip("/")
        for prefix in watched_prefixes:
            if normalized == prefix or normalized.startswith(prefix + "/"):
                return True
    return False


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Detect changes for configured docs libraries")
    parser.add_argument("--config", required=True, type=Path, help="Path to libraries JSON config")
    parser.add_argument("--event-name", required=True, help="GitHub event name")
    parser.add_argument("--before-sha", default="", help="Previous commit SHA")
    parser.add_argument("--current-sha", default="", help="Current commit SHA")
    return parser.parse_args()


def main() -> int:
    args = parse_args()

    output_raw = os.environ.get("GITHUB_OUTPUT", "")
    if not output_raw:
        print("[detect] GITHUB_OUTPUT is not set", file=sys.stderr)
        return 1
    output_path = Path(output_raw)

    if args.event_name == "workflow_dispatch":
        write_output(output_path, "should_run", "true")
        write_output(output_path, "reason", "manual dispatch")
        print("[detect] workflow_dispatch: run docs build")
        return 0

    watched_prefixes = build_watched_prefixes(args.config)
    changed_files = run_git_diff(args.before_sha, args.current_sha)

    should_run = should_run_for_changes(changed_files, watched_prefixes)

    write_output(output_path, "should_run", "true" if should_run else "false")
    write_output(
        output_path,
        "reason",
        "library files changed" if should_run else "no configured library changes",
    )

    print(f"[detect] should_run={should_run}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
