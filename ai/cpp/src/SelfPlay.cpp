#include "SelfPlay.hpp"
#include <random>
#include <fstream>

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
