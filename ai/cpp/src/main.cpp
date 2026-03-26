#include "SelfPlay.hpp"
#include <iostream>
#include <string>

int main() {
    constexpr int kNumGames = 1000;
    const std::string csv_path = "ai/data/game.csv";
    const std::string old_bin_path = "ai/data/game.bin";
    const std::string dataset_path = "ai/data/dataset.bin";

    size_t total_positions = 0;
    for (int i = 0; i < kNumGames; ++i) {
        SelfPlay sp;
        sp.play_random_game();

        // 既存動作確認向け出力（最終ゲームのみ上書き）。
        if (i == kNumGames - 1) {
            sp.save_csv(csv_path);
            sp.save_binary(old_bin_path);
        }

        // 学習用データセットは1ファイルに追記していく。
        sp.append_training_binary(dataset_path);
        total_positions += sp.history.size();
    }

    std::cout << "Random self-play finished.\n";
    std::cout << "Games: " << kNumGames << "\n";
    std::cout << "Total positions appended: " << total_positions << "\n";
    std::cout << "Output: " << dataset_path << std::endl;
    return 0;
}
