#pragma once
#include "BitBoard.hpp"
#include <cstdint>
#include <vector>
#include <string>
#include <random>

// 1ゲーム分の盤面・手・勝敗を記録し、自己対戦を行う
class SelfPlay {
public:
    enum class TieBreakMode {
        FixedMinIndex,
        RandomAmongBest,
    };

    struct Record {
        uint64_t black;
        uint64_t white;
        bool black_to_move;
        int move; // 0~63, パスは-1
    };

    std::vector<Record> history;
    int result; // 1:黒勝ち, -1:白勝ち, 0:引き分け

    // dataset.binへ保存する固定長レコード（19バイト）
    struct TrainingRecord {
        uint64_t black;
        uint64_t white;
        int8_t black_to_move;
        int8_t move;
        int8_t result;
    };

    // ランダム自己対戦を実行
    void play_random_game();

    // 学習済みモデルの推論値で最善手を選ぶ自己対戦
    void play_model_guided_game(
        const std::string& model_path,
        const std::string& inference_script,
        TieBreakMode tie_break_mode = TieBreakMode::FixedMinIndex,
        uint32_t random_seed = 42
    );

    // 黒白で異なるモデルを使って対戦する評価用モード
    void play_duel_game(
        const std::string& black_model_path,
        const std::string& white_model_path,
        const std::string& inference_script,
        TieBreakMode tie_break_mode = TieBreakMode::FixedMinIndex,
        uint32_t random_seed = 42
    );

    // CSVファイルに出力
    void save_csv(const std::string& filename) const;

    // バイナリファイルに出力
    void save_binary(const std::string& filename) const;

    // dataset.bin向けに固定長レコードを追記出力
    void append_training_binary(const std::string& filename) const;

private:
    int select_best_move_by_model(
        const BitBoard& board,
        uint64_t legal,
        const std::string& model_path,
        const std::string& inference_script,
        TieBreakMode tie_break_mode,
        std::mt19937& gen
    ) const;
};
