# 07. トラブルシューティング集

この章は「困ったときに最短で復旧するための辞書」です。

## 1. C++: gtest include エラー (1696)

症状:

- `#include <gtest/gtest.h>` に赤波線
- `include エラーが検出されました` などが出る

主な原因:

- gtest ヘッダパスを IntelliSense が知らない
- compile_commands.json 未生成
- CMake 未 configure

対処手順:

1. `make test-cpp` を実行して build を作る
2. `tester/cpp/build/compile_commands.json` の存在を確認
3. `.vscode/settings.json` の compileCommands 設定を確認
4. `.vscode/c_cpp_properties.json` の includePath を確認
5. `C/C++: Reset IntelliSense Database` を実行

---

## 2. C++ TestMate にケースが表示されない

症状:

- Testing View に C++ テストが出ない

主な原因:

- テストバイナリが未生成
- TestMate の探索パターンが不一致
- 拡張未インストール

対処手順:

1. C++ TestMate 拡張が有効か確認
2. `make test-cpp` 実行
3. `.vscode/settings.json` の `testMate.cpp.test.advancedExecutables` を確認
4. Testing View を再読み込み

---

## 3. pytest が 0 件になる

症状:

- `collected 0 items`

主な原因:

- ファイル名/関数名が規約外
- 探索ディレクトリ設定が違う
- import 失敗で読み込み落ち

対処手順:

1. ファイル名を `test_*.py` にする
2. 関数名を `test_*` にする
3. `PYTHONPATH=. pytest -q tester/python` を実行
4. import エラーが出ていないか確認

---

## 4. CMake configure で gtest 取得失敗

症状:

- FetchContent でダウンロード失敗

主な原因:

- ネットワーク制限
- 一時的な GitHub 側障害

対処手順:

1. ネットワーク再確認
2. しばらく待って再実行
3. 必要なら gtest をミラー/サブモジュール運用に変更

---

## 5. Death Test が通らない

症状:

- `EXPECT_DEATH` が失敗

主な原因:

- Release ビルドで assert 無効 (`NDEBUG`)
- 実装が例外化されている

対処手順:

1. Debug ビルドで実行
2. `#ifndef NDEBUG` でガードされているか確認
3. assert 依存ではなく例外仕様ならテスト戦略を変更

---

## 6. Python と C++ で仕様不一致

症状:

- Python テストは通るが C++ テストは失敗 (逆もある)

主な原因:

- 片方だけ実装修正
- エラー時動作の仕様ズレ

対処手順:

1. `library/library.py` と `library/library.hpp` の仕様を比較
2. 想定動作を README や docs に明文化
3. 双方テストを同じ観点で更新

---

## 7. build が壊れている疑い

症状:

- 以前通っていたのに突然 C++ 側だけ不安定

対処手順:

```bash
rm -rf tester/cpp/build
make test-cpp
```

キャッシュ破損は C++ では頻出です。再生成が最短復旧になることが多いです。

---

## 8. 最終手段チェックリスト

- [ ] 拡張は有効か
- [ ] CMake configure は済んでいるか
- [ ] compile_commands.json はあるか
- [ ] テストバイナリは生成されているか
- [ ] pytest/gtest の命名規約を守っているか
- [ ] 実装とテストの仕様が一致しているか

このチェックで大半の問題は解決できます。
