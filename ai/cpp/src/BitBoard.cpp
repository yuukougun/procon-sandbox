#include "BitBoard.hpp"
#include <bitset>

// 8方向のシフト量
constexpr int DIRS[8] = {1, -1, 8, -8, 9, -9, 7, -7};

// 盤面初期化
BitBoard::BitBoard() {
    reset();
}

BitBoard::BitBoard(uint64_t b, uint64_t w, bool turn) : black(b), white(w), black_to_move(turn) {}

void BitBoard::reset() {
    // 初期配置
    black = 0x0000000810000000ULL;
    white = 0x0000001008000000ULL;
    black_to_move = true;
}

// 合法手判定（簡易版: 8方向ビット演算）
uint64_t BitBoard::get_legal_moves() const {
    uint64_t player = black_to_move ? black : white;
    uint64_t opponent = black_to_move ? white : black;
    uint64_t empty = ~(black | white);
    uint64_t legal = 0;
    for (int d = 0; d < 8; ++d) {
        int dir = DIRS[d];
        uint64_t mask = 0;
        uint64_t tmp = player;
        for (int i = 0; i < 6; ++i) {
            // 1方向に相手石をたどる
            if (dir > 0)
                tmp = (tmp << dir) & opponent;
            else
                tmp = (tmp >> -dir) & opponent;
            mask |= tmp;
        }
        if (dir > 0)
            legal |= (mask << dir) & empty;
        else
            legal |= (mask >> -dir) & empty;
    }
    return legal;
}

// 指定位置に着手し石を反転
void BitBoard::do_move(int pos) {
    uint64_t move = 1ULL << pos;
    uint64_t player = black_to_move ? black : white;
    uint64_t opponent = black_to_move ? white : black;
    uint64_t flipped = 0;
    for (int d = 0; d < 8; ++d) {
        int dir = DIRS[d];
        uint64_t mask = 0;
        uint64_t cur = move;
        for (int i = 0; i < 6; ++i) {
            if (dir > 0)
                cur = (cur << dir) & opponent;
            else
                cur = (cur >> -dir) & opponent;
            if (!cur) break;
            mask |= cur;
        }
        // 挟めたか判定
        uint64_t end;
        if (dir > 0)
            end = (cur << dir) & player;
        else
            end = (cur >> -dir) & player;
        if (end) flipped |= mask;
    }
    if (black_to_move) {
        black ^= move | flipped;
        white ^= flipped;
    } else {
        white ^= move | flipped;
        black ^= flipped;
    }
    black_to_move = !black_to_move;
}

bool BitBoard::is_game_over() const {
    // 両者合法手なしでゲーム終了
    if (get_legal_moves() != 0) return false;
    BitBoard tmp(*this);
    tmp.black_to_move = !black_to_move;
    if (tmp.get_legal_moves() != 0) return false;
    return true;
}

int BitBoard::count_black() const {
    return std::bitset<64>(black).count();
}
int BitBoard::count_white() const {
    return std::bitset<64>(white).count();
}

std::vector<int> BitBoard::to_array() const {
    std::vector<int> arr(64, 0);
    for (int i = 0; i < 64; ++i) {
        if ((black >> i) & 1) arr[i] = 1;
        else if ((white >> i) & 1) arr[i] = -1;
    }
    return arr;
}
