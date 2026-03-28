#pragma once
#include <cstdint>
#include <vector>

// 8x8オセロ盤面をビットボードで管理
class BitBoard {
public:
    // 盤面状態
    uint64_t black;
    uint64_t white;
    bool black_to_move;

    BitBoard();
    BitBoard(uint64_t black, uint64_t white, bool black_to_move = true);

    // 合法手をビットボードで返す
    uint64_t get_legal_moves() const;

    // 指定位置に着手し、石を反転
    void do_move(int pos); // pos: 0~63

    // ゲーム終了判定
    bool is_game_over() const;

    // 石数カウント
    int count_black() const;
    int count_white() const;

    // 盤面を初期化
    void reset();

    // 盤面状態を配列で取得（デバッグ・可視化用）
    std::vector<int> to_array() const;
};
