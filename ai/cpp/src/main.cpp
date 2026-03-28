#include "SelfPlay.hpp"
#include <iostream>
#include <string>
#include <exception>

namespace {
SelfPlay::TieBreakMode parse_tie_break(const std::string& s) {
    if (s == "random") return SelfPlay::TieBreakMode::RandomAmongBest;
    return SelfPlay::TieBreakMode::FixedMinIndex;
}

SelfPlay::ModelSideMode parse_model_side(const std::string& s) {
    if (s == "black") return SelfPlay::ModelSideMode::BlackModelOnly;
    if (s == "white") return SelfPlay::ModelSideMode::WhiteModelOnly;
    return SelfPlay::ModelSideMode::BothModel;
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
    const int num_games = get_arg_int(argc, argv, "--num-games", 1);
    const std::string dataset_path = get_arg(argc, argv, "--dataset", "ai/data/dataset.bin");
    const std::string model_path = get_arg(argc, argv, "--model", "");
    const std::string model_black = get_arg(argc, argv, "--model-black", "");
    const std::string model_white = get_arg(argc, argv, "--model-white", "");
    const std::string inference_script = get_arg(argc, argv, "--inference-script", "ai/python/value_inference_wrapper.py");
    const auto tie_break_mode = parse_tie_break(get_arg(argc, argv, "--tie-break", "fixed"));
    const auto model_side_mode = parse_model_side(get_arg(argc, argv, "--model-side", "black"));
    const int beam_width = get_arg_int(argc, argv, "--beam-width", 32);
    const int beam_top_k = get_arg_int(argc, argv, "--beam-top-k", 2);
    const int log_interval = get_arg_int(argc, argv, "--log-interval", 10);
    const int seed = get_arg_int(argc, argv, "--seed", 42);

    if (num_games != 1) {
        std::cerr << "warning: data generation is fixed to 1 game. --num-games is ignored.\n";
    }

    std::string black_model_path;
    std::string white_model_path;

    if (mode == "random") {
        // 両者ランダムでビーム展開
        black_model_path = "";
        white_model_path = "";
    } else if (mode == "guided") {
        if (model_path.empty()) {
            std::cerr << "guided mode requires --model\n";
            return 1;
        }
        if (model_side_mode == SelfPlay::ModelSideMode::BlackModelOnly) {
            black_model_path = model_path;
            white_model_path = "";
        } else if (model_side_mode == SelfPlay::ModelSideMode::WhiteModelOnly) {
            black_model_path = "";
            white_model_path = model_path;
        } else {
            black_model_path = model_path;
            white_model_path = model_path;
        }
    } else if (mode == "duel") {
        if (model_black.empty() || model_white.empty()) {
            std::cerr << "duel mode requires --model-black and --model-white\n";
            return 1;
        }
        black_model_path = model_black;
        white_model_path = model_white;
    } else {
        std::cerr << "unsupported mode: " << mode << " (use random|guided|duel)\n";
        return 1;
    }

    try {
        SelfPlay sp;
        auto [generated_games, positions] = sp.append_training_binary_with_beam(
            dataset_path,
            inference_script,
            1,
            beam_width,
            beam_top_k,
            black_model_path,
            white_model_path,
            tie_break_mode,
            log_interval,
            static_cast<uint32_t>(seed)
        );

        std::cout << mode << " self-play (beam) finished.\n";
        std::cout << "Games: " << generated_games << "\n";
        std::cout << "Total positions appended: " << positions << "\n";
        std::cout << "Output: " << dataset_path << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "fatal: " << e.what() << "\n";
        return 1;
    }
}
