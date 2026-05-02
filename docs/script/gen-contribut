#!/usr/bin/env bash
# エラーが発生した場合、未定義の変数を参照した場合、またはパイプの途中でエラーがあった場合にスクリプトを即座に終了させるオプション
set -euo pipefail

# Gitコマンドを利用して、リポジトリのルートディレクトリの絶対パスを取得する
REPO_ROOT=$(git rev-parse --show-toplevel)
# ルートとなる目次（ナビゲーション）ファイルの絶対パスを定義
ROOT_NAV="${REPO_ROOT}/docs/_navigation.md"

# 対象の目次ファイルが存在しない場合は、標準エラー出力にメッセージを出してエラー終了
if [[ ! -f "$ROOT_NAV" ]]; then
    echo "docs/_navigation.md が見つかりません。" >&2
    exit 1
fi

# スクリプト内で生成する一時ファイルのパスを保持する配列
tmp_files=()

# スクリプト終了時に呼ばれるクリーンアップ関数（一時ファイルの削除）
cleanup() {
    if ((${#tmp_files[@]} > 0)); then
        rm -f "${tmp_files[@]}"
    fi
}
# スクリプトが終了（正常終了、エラー終了、シグナル中断など）する際に常に cleanup 関数を実行するようトラップを設定
trap cleanup EXIT

# 指定されたナビゲーションファイルを解析し、ドキュメントの「タイトル」と「パス」を抽出する関数
# 例: - [利用手順](/docs/hoge.md) をパースする
parse_nav_entries() {
    local nav_file="$1"
    while IFS= read -r line; do
        # 正規表現を使用して Markdown のリストリンク形式 ` - [タイトル](パス)` にマッチさせる
        if [[ "$line" =~ ^[[:space:]]*-[[:space:]]\[(.+)\]\((.+)\)$ ]]; then
            # 抽出した「タイトル」と「パス」をタブ文字(\t)区切りで標準出力に出力
            printf "%s\t%s\n" "${BASH_REMATCH[1]}" "${BASH_REMATCH[2]}"
        fi
    done < "$nav_file"
}

# 目次一覧の中で、「現在読んでいるドキュメント」の行に " - now" という文字列を追記して目立たせる関数
mark_current_doc() {
    local nav_file="$1"
    local target_doc_name="$2"
    
    # awkを使用して対象ファイルを1行ずつ処理する
    awk -v target="$target_doc_name" '
    /^[[:space:]]*- \[/ {
        # 目次のリンク行にマッチした場合、その行が現在のドキュメントを指しているか検査
        line = $0
        sub(/^[[:space:]]*- \[/, "", line)
        split(line, pieces, /]\(/)
        
        # pieces[1] にはタイトル部分が入っている。これがターゲットと一致したら末尾に " - now" をつける
        if (pieces[1] == target) {
            print $0 " - now"
            next
        }
    }
    {
        # マッチしなかった行はそのまま出力
        print
    }
    ' "$nav_file"
}

# パス（相対パスまたは絶対パス）をリポジトリルートからの絶対パスに変換・正規化する関数
resolve_doc_path() {
    local rel_path="$1"
    # パスが "/" から始まる場合はすでに絶対パス的な記述（リポジトリルートからのパスを意図している）
    if [[ "$rel_path" = /* ]]; then
        printf "%s%s\n" "$REPO_ROOT" "$rel_path"
    else
        printf "%s/%s\n" "$REPO_ROOT" "$rel_path"
    fi
}

# 個別の Markdown ドキュメント内に記述された目次ブロックを更新する関数
update_doc_navigation() {
    local nav_file="$1"
    
    # parse_nav_entries で抽出したタブ区切りのタイトルとパスをループで受け取る
    while IFS=$'\t' read -r doc_name doc_rel_path; do
        # タイトルやパスが空の場合はスキップ
        [[ -z "${doc_name:-}" || -z "${doc_rel_path:-}" ]] && continue

        local doc_path
        doc_path=$(resolve_doc_path "$doc_rel_path")
        # 実在するファイルでない場合は警告を出してスキップ
        if [[ ! -f "$doc_path" ]]; then
            echo "スキップ: ドキュメントが見つかりません ($doc_rel_path)" >&2
            continue
        fi

        # 置換用の目次テキストや、置換されたドキュメント全体を保持する一時ファイルを作成
        local nav_tmp doc_tmp
        nav_tmp=$(mktemp)
        doc_tmp=$(mktemp)
        tmp_files+=("$nav_tmp" "$doc_tmp")

        # 現在のドキュメント名を対象に、" - now" のマークアップを入れた目次一覧を生成して一時ファイルに保存
        mark_current_doc "$nav_file" "$doc_name" > "$nav_tmp"

        # 目次一覧の後に、本文のヘッダーとなる `## ドキュメントタイトル` を追記しておく
        printf "\n## %s\n" "$doc_name" >> "$nav_tmp"

        # awkを用いて、各ドキュメントファイル内の `BEGIN-NAVIGATION` と `END-NAVIGATION` の間の内容を丸ごと差し替える
        awk -v nav="$nav_tmp" '
        /BEGIN-NAVIGATION/ {
            print # BEGIN-NAVIGATION という目印の行自体は残す
            # 準備した最新の目次情報ファイルを内容まるごと差し込む
            while ((getline line < nav) > 0) {
                print line
            }
            close(nav)
            # skipフラグを立てて、元の古い目次記述が出力されないようにする
            skip=1
            next
        }
        /END-NAVIGATION/ {
            # 目印を見つけたらスキップを解除し、以降の本文はそのまま出力されるようにする
            skip=0
        }
        !skip # skip中でなければ行を出力する
        ' "$doc_path" > "$doc_tmp"

        # 更新が完了した一時ファイルの内容で、元のドキュメントファイルを上書きする
        mv "$doc_tmp" "$doc_path"
    done < <(parse_nav_entries "$nav_file")
}

# 1. docs/ 配下にある全ての `_navigation.md` (目次定義ファイル) を探索して配列に格納する
mapfile -t navigation_files < <(find "${REPO_ROOT}/docs" -type f -name "_navigation.md" | sort)
# 2. 各目次定義について、所属するドキュメント内の目次ブロックを更新するループを回す
for nav_file in "${navigation_files[@]}"; do
    update_doc_navigation "$nav_file"
done

# === ここからは全ドキュメントを1つにまとめた CONTRIBUTING.md を生成する処理 ===

# 集約ドキュメント用の一時ファイルを作成
contribut_tmp=$(mktemp)
tmp_files+=("$contribut_tmp")

# ヘッダー部分と、全体の目次（ルートの_navigation.mdデータ）をそのまま出力する
{
    echo "<!-- このファイルは自動生成されるものです。書き換えても内容は破棄されます。-->"
    echo "# proconでのリポジトリの使い方"
    echo
    cat "$ROOT_NAV"
} > "$contribut_tmp"

# ルート目次に記載されている順番にドキュメントを読み込み、折りたたみ(アコーディオン)形式で全コンテンツを結合する
while IFS=$'\t' read -r doc_name doc_rel_path; do
    [[ -z "${doc_name:-}" || -z "${doc_rel_path:-}" ]] && continue

    doc_path=$(resolve_doc_path "$doc_rel_path")
    if [[ ! -f "$doc_path" ]]; then
        echo "スキップ: ドキュメントが見つかりません ($doc_rel_path)" >&2
        continue
    fi

    # アコーディオン（HTMLの details / summary タグ）の開始部分を作成
    {
        echo
        echo "<a id=\"$doc_name\"></a>"
        echo "<details>"
        echo "<summary><span style=\"font-size:1.5em; font-weight:bold;\">$doc_name</span></summary>"
        echo
        echo "---"
    } >> "$contribut_tmp"

    # 各ドキュメント内から本文だけを抽出する。
    # sed -n '/END-NAVIGATION/,$p' により END-NAVIGATION 以降の行を取り出し、
    # sed '1d' によって最初の行（"END-NAVIGATION"という文字列の行自身）を削除することで純粋な本文が取り出せる。
    sed -n '/END-NAVIGATION/,$p' "$doc_path" | sed '1d' >> "$contribut_tmp"

    # アコーディオン要素を閉じる
    {
        echo "</details>"
        echo
        echo "---"
    } >> "$contribut_tmp"
# プロセス置換 <() を使用して、parse_nav_entries の出力を while ループに渡す（サブシェル問題の回避）
done < <(parse_nav_entries "$ROOT_NAV")

# 生成した集約ドキュメントをリポジトリルートの CONTRIBUTING.md として上書きする
mv "$contribut_tmp" "${REPO_ROOT}/CONTRIBUTING.md"
