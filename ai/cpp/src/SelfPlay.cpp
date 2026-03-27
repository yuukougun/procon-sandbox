#include "SelfPlay.hpp"
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <limits>
#include <cmath>

#include <unistd.h>

namespace {
int8_t to_int8_safely(int v) {
    return static_cast<int8_t>(v);
}

std::string make_tmp_path(const std::string& prefix) {
    std::ostringstream oss;
    oss << "/tmp/" << prefix << "_" << getpid() << "_" << std::rand() << ".txt";
    return oss.str();
}

int decide_winner(const BitBoard& board) {
    int b = board.count_black();
    int w = board.count_white();
    return (b > w) ? 1 : (b < w) ? -1 : 0;
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
    result = decide_winner(board);
}

int SelfPlay::select_best_move_by_model(
    const BitBoard& board,
    uint64_t legal,
    const std::string& model_path,
    const std::string& inference_script,
    TieBreakMode tie_break_mode,
    std::mt19937& gen
) const {
    std::vector<int> moves;
    moves.reserve(32);
    std::vector<BitBoard> next_states;
    next_states.reserve(32);

    for (int i = 0; i < 64; ++i) {
        if (((legal >> i) & 1ULL) == 0) continue;
        moves.push_back(i);
        BitBoard next = board;
        next.do_move(i);
        next_states.push_back(next);
    }

    if (moves.empty()) return -1;

    const std::string input_path = make_tmp_path("othello_in");
    const std::string output_path = make_tmp_path("othello_out");

    {
        std::ofstream ofs(input_path);
        ofs << moves.size() << "\n";
        for (const auto& st : next_states) {
            ofs << st.black << " " << st.white << " " << (st.black_to_move ? 1 : 0) << "\n";
        }
    }

    std::ostringstream cmd;
    cmd
        << "python " << inference_script
        << " --model " << model_path
        << " --input-file " << input_path
        << " --output-file " << output_path;

    int rc = std::system(cmd.str().c_str());
    if (rc != 0) {
        std::remove(input_path.c_str());
        std::remove(output_path.c_str());
        return moves.front();
    }

    std::vector<double> scores;
    {
        std::ifstream ifs(output_path);
        double s = 0.0;
        while (ifs >> s) scores.push_back(s);
    }

    std::remove(input_path.c_str());
    std::remove(output_path.c_str());

    if (scores.size() != moves.size()) {
        return moves.front();
    }

    double best_score = -std::numeric_limits<double>::infinity();
    std::vector<int> best_indices;
    for (size_t i = 0; i < scores.size(); ++i) {
        double score = board.black_to_move ? scores[i] : -scores[i];
        if (score > best_score + 1e-12) {
            best_score = score;
            best_indices.clear();
            best_indices.push_back(static_cast<int>(i));
        } else if (std::abs(score - best_score) <= 1e-12) {
            best_indices.push_back(static_cast<int>(i));
        }
    }

    if (best_indices.empty()) return moves.front();
    if (tie_break_mode == TieBreakMode::FixedMinIndex) {
        int min_i = *std::min_element(best_indices.begin(), best_indices.end());
        return moves[min_i];
    }

    std::uniform_int_distribution<> dis(0, static_cast<int>(best_indices.size()) - 1);
    return moves[best_indices[dis(gen)]];
}

void SelfPlay::play_model_guided_game(
    const std::string& model_path,
    const std::string& inference_script,
    TieBreakMode tie_break_mode,
    uint32_t random_seed
) {
    history.clear();
    BitBoard board;
    std::mt19937 gen(random_seed);

    while (!board.is_game_over()) {
        uint64_t legal = board.get_legal_moves();
        int move = -1;
        if (legal) {
            move = select_best_move_by_model(board, legal, model_path, inference_script, tie_break_mode, gen);
            board.do_move(move);
        } else {
            board.black_to_move = !board.black_to_move;
        }
        history.push_back({board.black, board.white, board.black_to_move, move});
    }

    result = decide_winner(board);
}

void SelfPlay::play_duel_game(
    const std::string& black_model_path,
    const std::string& white_model_path,
    const std::string& inference_script,
    TieBreakMode tie_break_mode,
    uint32_t random_seed
) {
    history.clear();
    BitBoard board;
    std::mt19937 gen(random_seed);

    while (!board.is_game_over()) {
        uint64_t legal = board.get_legal_moves();
        int move = -1;
        if (legal) {
            const std::string& model_path = board.black_to_move ? black_model_path : white_model_path;
            move = select_best_move_by_model(board, legal, model_path, inference_script, tie_break_mode, gen);
            board.do_move(move);
        } else {
            board.black_to_move = !board.black_to_move;
        }
        history.push_back({board.black, board.white, board.black_to_move, move});
    }

    result = decide_winner(board);
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
