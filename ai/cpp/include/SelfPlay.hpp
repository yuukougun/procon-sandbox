#pragma once
#include "BitBoard.hpp"
#include <vector>
#include <string>

// 1ゲーム分の盤面・手・勝敗を記録し、自己対戦を行う
class SelfPlay {
public:
    struct Record {
        uint64_t black;
        uint64_t white;
        bool black_to_move;
        int move; // 0~63, パスは-1
    };

    std::vector<Record> history;
    int result; // 1:黒勝ち, -1:白勝ち, 0:引き分け

    // ランダム自己対戦を実行
    void play_random_game();

    // CSVファイルに出力
    void save_csv(const std::string& filename) const;
};
