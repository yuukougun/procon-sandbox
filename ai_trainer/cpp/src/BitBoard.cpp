#include "BitBoard.hpp"
#include <bitset>

namespace {
constexpr int DX[8] = {1, -1, 0, 0, 1, -1, 1, -1};
constexpr int DY[8] = {0, 0, 1, -1, 1, -1, -1, 1};

inline bool in_board(int x, int y) {
    return (0 <= x && x < 8 && 0 <= y && y < 8);
}

inline uint64_t bit_at(int pos) {
    return 1ULL << pos;
}
} // namespace

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

uint64_t BitBoard::get_legal_moves() const {
    uint64_t player = black_to_move ? black : white;
    uint64_t opponent = black_to_move ? white : black;
    uint64_t occupied = black | white;
    uint64_t legal = 0;

    for (int pos = 0; pos < 64; ++pos) {
        if (occupied & bit_at(pos)) continue;
        int x = pos % 8;
        int y = pos / 8;

        for (int d = 0; d < 8; ++d) {
            int nx = x + DX[d];
            int ny = y + DY[d];
            bool seen_opponent = false;

            while (in_board(nx, ny)) {
                int np = ny * 8 + nx;
                uint64_t b = bit_at(np);
                if (opponent & b) {
                    seen_opponent = true;
                    nx += DX[d];
                    ny += DY[d];
                    continue;
                }
                if (seen_opponent && (player & b)) {
                    legal |= bit_at(pos);
                }
                break;
            }
            if (legal & bit_at(pos)) break;
        }
    }
    return legal;
}

// 指定位置に着手し石を反転
void BitBoard::do_move(int pos) {
    if (pos < 0 || pos >= 64) {
        // パス
        black_to_move = !black_to_move;
        return;
    }
    uint64_t move = bit_at(pos);
    // 合法手チェック
    uint64_t legal = get_legal_moves();
    if ((move & legal) == 0) {
        // 非合法手は無視
        return;
    }
    uint64_t player = black_to_move ? black : white;
    uint64_t opponent = black_to_move ? white : black;
    uint64_t flipped = 0;

    int x = pos % 8;
    int y = pos / 8;
    for (int d = 0; d < 8; ++d) {
        int nx = x + DX[d];
        int ny = y + DY[d];
        std::vector<int> candidates;

        while (in_board(nx, ny)) {
            int np = ny * 8 + nx;
            uint64_t b = bit_at(np);
            if (opponent & b) {
                candidates.push_back(np);
                nx += DX[d];
                ny += DY[d];
                continue;
            }
            if (!candidates.empty() && (player & b)) {
                for (int cp : candidates) {
                    flipped |= bit_at(cp);
                }
            }
            break;
        }
    }

    if (black_to_move) {
        black |= move | flipped;
        white &= ~flipped;
    } else {
        white |= move | flipped;
        black &= ~flipped;
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
