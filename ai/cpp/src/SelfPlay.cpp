#include "SelfPlay.hpp"
#include <random>
#include <fstream>

namespace {
int8_t to_int8_safely(int v) {
    return static_cast<int8_t>(v);
}
} // namespace

void SelfPlay::play_random_game() {
    history.clear();
    BitBoard board;
    std::random_device rd;
    std::mt19937 gen(rd());
    while (!board.is_game_over()) {
        uint64_t legal = board.get_legal_moves();
        int move = -1;
        if (legal) {
            // 合法手のインデックスを列挙
            std::vector<int> moves;
            for (int i = 0; i < 64; ++i) {
                if ((legal >> i) & 1) moves.push_back(i);
            }
            std::uniform_int_distribution<> dis(0, moves.size() - 1);
            move = moves[dis(gen)];
            board.do_move(move);
        } else {
            // パス
            board.black_to_move = !board.black_to_move;
        }
        history.push_back({board.black, board.white, board.black_to_move, move});
    }
    // 勝敗判定
    int b = board.count_black();
    int w = board.count_white();
    result = (b > w) ? 1 : (b < w) ? -1 : 0;
}

void SelfPlay::save_csv(const std::string& filename) const {
    std::ofstream ofs(filename);
    ofs << "black,white,black_to_move,move,result\n";
    for (const auto& rec : history) {
        ofs << rec.black << "," << rec.white << "," << rec.black_to_move << "," << rec.move << "," << result << "\n";
    }
}

void SelfPlay::save_binary(const std::string& filename) const {
    std::ofstream ofs(filename, std::ios::binary);
    size_t n = history.size();
    ofs.write(reinterpret_cast<const char*>(&n), sizeof(n));
    ofs.write(reinterpret_cast<const char*>(&result), sizeof(result));
    for (const auto& rec : history) {
        ofs.write(reinterpret_cast<const char*>(&rec.black), sizeof(rec.black));
        ofs.write(reinterpret_cast<const char*>(&rec.white), sizeof(rec.white));
        ofs.write(reinterpret_cast<const char*>(&rec.black_to_move), sizeof(rec.black_to_move));
        ofs.write(reinterpret_cast<const char*>(&rec.move), sizeof(rec.move));
    }
}

void SelfPlay::append_training_binary(const std::string& filename) const {
    // 既存ファイル末尾に1局面ずつ追記する。
    std::ofstream ofs(filename, std::ios::binary | std::ios::app);
    for (const auto& rec : history) {
        TrainingRecord out{
            rec.black,
            rec.white,
            to_int8_safely(rec.black_to_move ? 1 : 0),
            to_int8_safely(rec.move),
            to_int8_safely(result),
        };
        // パディング依存を避けるため、各フィールドを逐次書き込みする。
        ofs.write(reinterpret_cast<const char*>(&out.black), sizeof(out.black));
        ofs.write(reinterpret_cast<const char*>(&out.white), sizeof(out.white));
        ofs.write(reinterpret_cast<const char*>(&out.black_to_move), sizeof(out.black_to_move));
        ofs.write(reinterpret_cast<const char*>(&out.move), sizeof(out.move));
        ofs.write(reinterpret_cast<const char*>(&out.result), sizeof(out.result));
    }
}
