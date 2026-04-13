# コマンド解説

## run.sh の使い方

`run.sh` はすべてのコマンド操作を行うスクリプトです。

```sh
./run.sh [コマンド]
```

---

## コマンド一覧

| コマンド | 効果 |
| --- | --- |
| `./run.sh build` | ソルブファイル（solve/solve.cpp）をコンパイル |
| `./run.sh clean` | ビルドファイル（build/）を削除 |
| `./run.sh run` | ソルブをコンパイルして実行 |
| `./run.sh test` | tester/testAll.py を実行しテストを行う |
| `./run.sh doxygen` | Doxyfile に基づきドキュメントを生成 |

---

## Makefile の使い方

`make` コマンドから直接操作することも可能です。

| コマンド | 効果 |
| --- | --- |
| `make` または `make all` | ソルブファイルをコンパイル |
| `make clean` | ビルドファイルを削除 |
| `make run` | ソルブをコンパイルして実行 |
| `make test` | テストを実行 |
