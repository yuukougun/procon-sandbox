#!/bin/bash
# すべてのコマンド操作を行うファイル
# 詳細はCOMMAND.mdを参照

set -e

COMMAND=$1

case $COMMAND in
    build)
        make all
        ;;
    clean)
        make clean
        ;;
    run)
        make run
        ;;
    test)
        make test
        ;;
    doxygen)
        doxygen Doxyfile
        ;;
    *)
        echo "使用方法: ./run.sh [コマンド]"
        echo ""
        echo "コマンド一覧:"
        echo "  build    - ソルブファイルをコンパイル"
        echo "  clean    - ビルドファイルを削除"
        echo "  run      - ソルブを実行"
        echo "  test     - テストを実行"
        echo "  doxygen  - ドキュメントを生成"
        echo ""
        echo "詳細はCOMMAND.mdを参照してください。"
        exit 1
        ;;
esac
