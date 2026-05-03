#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "../../cpp/include/BitBoard.hpp"
#include <sstream>


struct Record {
    uint64_t black;
    uint64_t white;
    bool black_to_move;
    int move;
};

// CSVファイルからmove列のみを読み込む
std::vector<int> load_moves(const std::string& filename) {
    std::vector<int> moves;
    std::ifstream ifs(filename);
    std::string line;
    std::getline(ifs, line); // ヘッダスキップ
    while (std::getline(ifs, line)) {
        std::stringstream ss(line);
        std::string buf;
        // black,white,black_to_move,move,result
        for (int i = 0; i < 3; ++i) std::getline(ss, buf, ',');
        std::getline(ss, buf, ',');
        int move = std::stoi(buf);
        moves.push_back(move);
    }
    return moves;
}

void print_board(const Record& rec) {
    std::cout << "+-----------------+\n";
    for (int y = 0; y < 8; ++y) {
        std::cout << "| ";
        for (int x = 0; x < 8; ++x) {
            int i = y * 8 + x;
            char c = '.';
            if ((rec.black >> i) & 1) c = 'B';
            else if ((rec.white >> i) & 1) c = 'W';
            std::cout << c << ' ';
        }
        std::cout << "|\n";
    }
    std::cout << "+-----------------+\n";
    std::cout << (rec.black_to_move ? "Black" : "White") << " to move\n";
}

int main(int argc, char* argv[]) {
    std::string filename = "ai/data/game.csv";
    if (argc > 1) filename = argv[1];
    auto moves = load_moves(filename);
    if (moves.empty()) {
        std::cerr << "No data loaded.\n";
        return 1;
    }
    // BitBoardで初期盤面からmove列を順次do_moveで再現
    std::vector<Record> history;
    BitBoard board;
    history.push_back({board.black, board.white, board.black_to_move, -1});
    for (int mv : moves) {
        if (mv == -1) {
            // パス
            board.black_to_move = !board.black_to_move;
        } else if (mv >= 0 && mv < 64) {
            board.do_move(mv);
        }
        history.push_back({board.black, board.white, board.black_to_move, mv});
    }
    size_t idx = 0;
    while (true) {
        system("clear");
        std::cout << "Move " << idx << "/" << (history.size() - 1) << "\n";
        print_board(history[idx]);
        std::cout << "[n]ext, [p]rev, [q]uit > ";
        char c;
        std::cin >> c;
        if (c == 'n' && idx + 1 < history.size()) ++idx;
        else if (c == 'p' && idx > 0) --idx;
        else if (c == 'q') break;
    }
    return 0;
}
