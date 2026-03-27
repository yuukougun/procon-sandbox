#include "SelfPlay.hpp"
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <limits>
#include <cmath>
#include <tuple>
#include <iostream>
#include <filesystem>
#include <stdexcept>

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

std::string quote_arg(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 2);
    out.push_back('"');
    for (char c : s) {
        if (c == '"' || c == '\\') out.push_back('\\');
        out.push_back(c);
    }
    out.push_back('"');
    return out;
}

std::string resolve_python_executable() {
    if (const char* forced = std::getenv("OTHELLO_PYTHON"); forced && *forced) {
        return std::string(forced);
    }

    if (const char* venv = std::getenv("VIRTUAL_ENV"); venv && *venv) {
        std::filesystem::path py = std::filesystem::path(venv) / "bin" / "python";
        if (std::filesystem::exists(py)) {
            return py.string();
        }
    }

    return "python3";
}

int decide_winner(const BitBoard& board) {
    int b = board.count_black();
    int w = board.count_white();
    return (b > w) ? 1 : (b < w) ? -1 : 0;
}

bool model_plays_turn(SelfPlay::ModelSideMode mode, bool black_to_move) {
    if (mode == SelfPlay::ModelSideMode::BothModel) return true;
    if (mode == SelfPlay::ModelSideMode::BlackModelOnly) return black_to_move;
    return !black_to_move;
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

std::vector<SelfPlay::MoveScore> SelfPlay::evaluate_moves_by_model(
    const BitBoard& board,
    uint64_t legal,
    const std::string& model_path,
    const std::string& inference_script
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

    if (moves.empty()) return {};

    const std::string input_path = make_tmp_path("othello_in");
    const std::string output_path = make_tmp_path("othello_out");

    {
        std::ofstream ofs(input_path);
        ofs << moves.size() << "\n";
        for (const auto& st : next_states) {
            ofs << st.black << " " << st.white << " " << (st.black_to_move ? 1 : 0) << "\n";
        }
    }

    static const std::string python_exe = resolve_python_executable();

    std::ostringstream cmd;
    cmd
        << quote_arg(python_exe)
        << " " << quote_arg(inference_script)
        << " --model " << quote_arg(model_path)
        << " --input-file " << quote_arg(input_path)
        << " --output-file " << quote_arg(output_path);

    int rc = std::system(cmd.str().c_str());
    if (rc != 0) {
        std::remove(input_path.c_str());
        std::remove(output_path.c_str());
        throw std::runtime_error("python inference failed (non-zero exit): " + cmd.str());
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
        throw std::runtime_error("python inference output size mismatch");
    }

    std::vector<MoveScore> out;
    out.reserve(moves.size());
    for (size_t i = 0; i < moves.size(); ++i) {
        out.push_back(MoveScore{moves[i], scores[i]});
    }
    return out;
}

int SelfPlay::select_best_move_by_model(
    const BitBoard& board,
    uint64_t legal,
    const std::string& model_path,
    const std::string& inference_script,
    TieBreakMode tie_break_mode,
    std::mt19937& gen
) const {
    auto move_scores = evaluate_moves_by_model(board, legal, model_path, inference_script);
    if (move_scores.empty()) {
        for (int i = 0; i < 64; ++i) {
            if (((legal >> i) & 1ULL) != 0) return i;
        }
        return -1;
    }

    double best_score = -std::numeric_limits<double>::infinity();
    std::vector<int> best_indices;
    for (size_t i = 0; i < move_scores.size(); ++i) {
        double score = board.black_to_move ? move_scores[i].score : -move_scores[i].score;
        if (score > best_score + 1e-12) {
            best_score = score;
            best_indices.clear();
            best_indices.push_back(static_cast<int>(i));
        } else if (std::abs(score - best_score) <= 1e-12) {
            best_indices.push_back(static_cast<int>(i));
        }
    }

    if (best_indices.empty()) return move_scores.front().move;
    if (tie_break_mode == TieBreakMode::FixedMinIndex) {
        int min_i = *std::min_element(best_indices.begin(), best_indices.end());
        return move_scores[min_i].move;
    }

    std::uniform_int_distribution<> dis(0, static_cast<int>(best_indices.size()) - 1);
    return move_scores[best_indices[dis(gen)]].move;
}

void SelfPlay::play_model_guided_game(
    const std::string& model_path,
    const std::string& inference_script,
    ModelSideMode model_side_mode,
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
            if (model_plays_turn(model_side_mode, board.black_to_move)) {
                move = select_best_move_by_model(board, legal, model_path, inference_script, tie_break_mode, gen);
            } else {
                std::vector<int> moves;
                for (int i = 0; i < 64; ++i) {
                    if ((legal >> i) & 1ULL) moves.push_back(i);
                }
                std::uniform_int_distribution<> dis(0, static_cast<int>(moves.size()) - 1);
                move = moves[dis(gen)];
            }
            board.do_move(move);
        } else {
            board.black_to_move = !board.black_to_move;
        }
        history.push_back({board.black, board.white, board.black_to_move, move});
    }

    result = decide_winner(board);
}

std::pair<int, size_t> SelfPlay::append_training_binary_with_beam(
    const std::string& filename,
    const std::string& inference_script,
    int target_games,
    int beam_width,
    int top_k,
    const std::string& black_model_path,
    const std::string& white_model_path,
    TieBreakMode tie_break_mode,
    int log_interval,
    uint32_t random_seed
) {
    struct BeamNode {
        BitBoard board;
        std::vector<Record> hist;
        double priority;
    };

    auto clamp_int = [](int v, int lo, int hi) {
        return std::max(lo, std::min(hi, v));
    };

    target_games = std::max(1, target_games);
    beam_width = std::max(1, beam_width);
    top_k = std::max(1, top_k);
    log_interval = std::max(1, log_interval);

    std::mt19937 gen(random_seed);
    std::uniform_real_distribution<double> noise_dis(0.0, 1.0);
    const std::string python_exe = resolve_python_executable();

    auto make_root = [&]() {
        BeamNode n;
        n.board = BitBoard();
        n.hist.clear();
        n.priority = 0.0;
        return n;
    };

    auto append_one_game = [&](const std::vector<Record>& hist, int winner) -> size_t {
        history = hist;
        result = winner;
        append_training_binary(filename);
        return hist.size();
    };

    std::vector<BeamNode> frontier;
    frontier.push_back(make_root());

    int completed_games = 0;
    size_t total_positions = 0;

    std::cout << "[beam] start target_games=" << target_games
              << " beam_width=" << beam_width
              << " top_k=" << top_k
              << " log_interval=" << log_interval
              << " python=" << python_exe << "\n";

    int beam_loop = 0;
    while (completed_games < target_games) {
        ++beam_loop;
        std::cout << "[beam] loop=" << beam_loop
                  << " frontier_in=" << frontier.size()
                  << " completed_games=" << completed_games
                  << " total_positions=" << total_positions << "\n";

        std::vector<BeamNode> next_frontier;
        next_frontier.reserve(static_cast<size_t>(beam_width) * static_cast<size_t>(top_k));

        for (auto& node : frontier) {
            if (completed_games >= target_games) break;

            if (node.board.is_game_over()) {
                int winner = decide_winner(node.board);
                total_positions += append_one_game(node.hist, winner);
                ++completed_games;
                continue;
            }

            uint64_t legal = node.board.get_legal_moves();
            if (legal == 0ULL) {
                BeamNode child = node;
                child.board.black_to_move = !child.board.black_to_move;
                child.hist.push_back({child.board.black, child.board.white, child.board.black_to_move, -1});
                next_frontier.push_back(std::move(child));
                continue;
            }

            const std::string& side_model_path = node.board.black_to_move ? black_model_path : white_model_path;

            std::vector<MoveScore> scored_moves;
            if (!side_model_path.empty()) {
                scored_moves = evaluate_moves_by_model(node.board, legal, side_model_path, inference_script);
                if (scored_moves.empty()) {
                    for (int i = 0; i < 64; ++i) {
                        if (((legal >> i) & 1ULL) != 0) {
                            scored_moves.push_back(MoveScore{i, noise_dis(gen)});
                        }
                    }
                }
            } else {
                for (int i = 0; i < 64; ++i) {
                    if (((legal >> i) & 1ULL) != 0) {
                        scored_moves.push_back(MoveScore{i, noise_dis(gen)});
                    }
                }
            }

            if (scored_moves.empty()) {
                continue;
            }

            std::shuffle(scored_moves.begin(), scored_moves.end(), gen);
            std::sort(scored_moves.begin(), scored_moves.end(), [&](const MoveScore& a, const MoveScore& b) {
                double sa = node.board.black_to_move ? a.score : -a.score;
                double sb = node.board.black_to_move ? b.score : -b.score;
                if (std::abs(sa - sb) <= 1e-12) {
                    if (tie_break_mode == TieBreakMode::FixedMinIndex) return a.move < b.move;
                    return false;
                }
                return sa > sb;
            });

            int expand_k = clamp_int(top_k, 1, static_cast<int>(scored_moves.size()));
            for (int j = 0; j < expand_k; ++j) {
                BeamNode child = node;
                int mv = scored_moves[static_cast<size_t>(j)].move;
                double raw_score = scored_moves[static_cast<size_t>(j)].score;
                child.board.do_move(mv);
                child.hist.push_back({child.board.black, child.board.white, child.board.black_to_move, mv});
                child.priority = child.priority + (node.board.black_to_move ? raw_score : -raw_score);
                next_frontier.push_back(std::move(child));
            }
        }

        if (completed_games >= target_games) break;

        if (next_frontier.empty()) {
            std::cout << "[beam] loop=" << beam_loop << " frontier_out=0 (reset root)\n";
            frontier.clear();
            frontier.push_back(make_root());
            continue;
        }

        std::sort(next_frontier.begin(), next_frontier.end(), [](const BeamNode& a, const BeamNode& b) {
            return a.priority > b.priority;
        });
        if (static_cast<int>(next_frontier.size()) > beam_width) {
            next_frontier.resize(static_cast<size_t>(beam_width));
        }

        frontier = std::move(next_frontier);

        std::cout << "[beam] loop=" << beam_loop
                  << " frontier_out=" << frontier.size()
                  << " completed_games=" << completed_games
                  << " total_positions=" << total_positions << "\n";

        while (static_cast<int>(frontier.size()) < beam_width && completed_games < target_games) {
            frontier.push_back(make_root());
        }
    }

    std::cout << "[beam] done completed_games=" << completed_games
              << " total_positions=" << total_positions << "\n";

    return {completed_games, total_positions};
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
