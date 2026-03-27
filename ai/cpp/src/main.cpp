#include "SelfPlay.hpp"
#include <iostream>
#include <string>

namespace {
SelfPlay::TieBreakMode parse_tie_break(const std::string& s) {
    if (s == "random") return SelfPlay::TieBreakMode::RandomAmongBest;
    return SelfPlay::TieBreakMode::FixedMinIndex;
}

std::string get_arg(int argc, char** argv, const std::string& key, const std::string& default_value) {
    for (int i = 1; i + 1 < argc; ++i) {
        if (argv[i] == key) return argv[i + 1];
    }
    return default_value;
}

int get_arg_int(int argc, char** argv, const std::string& key, int default_value) {
    return std::stoi(get_arg(argc, argv, key, std::to_string(default_value)));
}
}

int main(int argc, char** argv) {
    const std::string mode = get_arg(argc, argv, "--mode", "random");
    const int num_games = get_arg_int(argc, argv, "--num-games", 1000);
    const std::string csv_path = get_arg(argc, argv, "--csv", "ai/data/game.csv");
    const std::string old_bin_path = get_arg(argc, argv, "--legacy-bin", "ai/data/game.bin");
    const std::string dataset_path = get_arg(argc, argv, "--dataset", "ai/data/dataset.bin");
    const std::string model_path = get_arg(argc, argv, "--model", "");
    const std::string model_black = get_arg(argc, argv, "--model-black", "");
    const std::string model_white = get_arg(argc, argv, "--model-white", "");
    const std::string inference_script = get_arg(argc, argv, "--inference-script", "ai/python/value_inference_wrapper.py");
    const auto tie_break_mode = parse_tie_break(get_arg(argc, argv, "--tie-break", "fixed"));
    const int seed = get_arg_int(argc, argv, "--seed", 42);

    size_t total_positions = 0;

    if (mode == "duel") {
        if (model_black.empty() || model_white.empty()) {
            std::cerr << "duel mode requires --model-black and --model-white\n";
            return 1;
        }

        int black_wins = 0;
        int white_wins = 0;
        int draws = 0;
        for (int i = 0; i < num_games; ++i) {
            SelfPlay sp;
            sp.play_duel_game(model_black, model_white, inference_script, tie_break_mode, static_cast<uint32_t>(seed + i));
            if (sp.result > 0) ++black_wins;
            else if (sp.result < 0) ++white_wins;
            else ++draws;
        }

        std::cout << "duel finished\n";
        std::cout << "games=" << num_games
                  << " black_wins=" << black_wins
                  << " white_wins=" << white_wins
                  << " draws=" << draws << "\n";
        return 0;
    }

    for (int i = 0; i < num_games; ++i) {
        SelfPlay sp;
        if (mode == "guided") {
            if (model_path.empty()) {
                std::cerr << "guided mode requires --model\n";
                return 1;
            }
            sp.play_model_guided_game(model_path, inference_script, tie_break_mode, static_cast<uint32_t>(seed + i));
        } else {
            sp.play_random_game();
        }

        // 既存動作確認向け出力（最終ゲームのみ上書き）。
        if (i == num_games - 1) {
            sp.save_csv(csv_path);
            sp.save_binary(old_bin_path);
        }

        // 学習用データセットは1ファイルに追記していく。
        sp.append_training_binary(dataset_path);
        total_positions += sp.history.size();
    }

    std::cout << (mode == "guided" ? "Guided" : "Random") << " self-play finished.\n";
    std::cout << "Games: " << num_games << "\n";
    std::cout << "Total positions appended: " << total_positions << "\n";
    std::cout << "Output: " << dataset_path << std::endl;
    return 0;
}
