#!/usr/bin/env python3
"""すべてのテストを実行するスクリプト"""

import subprocess
import sys
import os

def run_test(name: str, command: list[str]) -> bool:
    """テストを実行し結果を返す"""
    print(f"[TEST] {name}")
    result = subprocess.run(command, capture_output=True, text=True)
    if result.returncode == 0:
        print(f"  [PASS] {name}")
        return True
    else:
        print(f"  [FAIL] {name}")
        if result.stdout:
            print(f"  stdout: {result.stdout}")
        if result.stderr:
            print(f"  stderr: {result.stderr}")
        return False

def main():
    passed = 0
    failed = 0

    # コンパイルテスト
    solve_dir = os.path.join(os.path.dirname(__file__), "..", "solve")
    solve_cpp = os.path.join(solve_dir, "solve.cpp")

    if run_test("solve.cpp コンパイル", ["g++", "-std=c++20", "-O2", "-o", "/tmp/solve", solve_cpp]):
        passed += 1
    else:
        failed += 1

    print(f"\n結果: {passed}件成功, {failed}件失敗")
    if failed > 0:
        sys.exit(1)

if __name__ == "__main__":
    main()
