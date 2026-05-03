#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <cstdint>
#include <string>
#include <vector>

#include "BitBoard.hpp"
#include "SelfPlay.hpp"

namespace py = pybind11;

namespace {
SelfPlay::TieBreakMode parse_tie_break(const std::string& s) {
    if (s == "random") return SelfPlay::TieBreakMode::RandomAmongBest;
    if (s == "fixed") return SelfPlay::TieBreakMode::FixedMinIndex;
    throw std::invalid_argument("tie_break must be 'fixed' or 'random'");
}

SelfPlay::ModelSideMode parse_model_side(const std::string& s) {
    if (s == "black") return SelfPlay::ModelSideMode::BlackModelOnly;
    if (s == "white") return SelfPlay::ModelSideMode::WhiteModelOnly;
    if (s == "both") return SelfPlay::ModelSideMode::BothModel;
    throw std::invalid_argument("model_side must be 'black', 'white', or 'both'");
}

std::vector<int> legal_move_indices(const BitBoard& board) {
    std::vector<int> out;
    uint64_t legal = board.get_legal_moves();
    for (int i = 0; i < 64; ++i) {
        if (((legal >> i) & 1ULL) != 0) out.push_back(i);
    }
    return out;
}

py::dict generate_dataset_beam(
    const std::string& mode,
    const std::string& dataset_path,
    const std::string& inference_script,
    const std::string& model_path,
    const std::string& model_black,
    const std::string& model_white,
    const std::string& model_side,
    int beam_width,
    int beam_top_k,
    const std::string& tie_break,
    int log_interval,
    uint32_t seed
) {
    std::string black_model_path;
    std::string white_model_path;

    if (mode == "random") {
        black_model_path.clear();
        white_model_path.clear();
    } else if (mode == "guided") {
        if (model_path.empty()) {
            throw std::invalid_argument("guided mode requires model_path");
        }
        auto side_mode = parse_model_side(model_side);
        if (side_mode == SelfPlay::ModelSideMode::BlackModelOnly) {
            black_model_path = model_path;
            white_model_path.clear();
        } else if (side_mode == SelfPlay::ModelSideMode::WhiteModelOnly) {
            black_model_path.clear();
            white_model_path = model_path;
        } else {
            black_model_path = model_path;
            white_model_path = model_path;
        }
    } else if (mode == "duel") {
        if (model_black.empty() || model_white.empty()) {
            throw std::invalid_argument("duel mode requires model_black and model_white");
        }
        black_model_path = model_black;
        white_model_path = model_white;
    } else {
        throw std::invalid_argument("unsupported mode: use random|guided|duel");
    }

    SelfPlay sp;
    auto [generated_games, positions] = sp.append_training_binary_with_beam(
        dataset_path,
        inference_script,
        1,
        beam_width,
        beam_top_k,
        black_model_path,
        white_model_path,
        parse_tie_break(tie_break),
        log_interval,
        seed
    );

    py::dict out;
    out["mode"] = mode;
    out["generated_games"] = generated_games;
    out["positions"] = positions;
    out["dataset_path"] = dataset_path;
    return out;
}

int play_duel_once(
    const std::string& model_black,
    const std::string& model_white,
    const std::string& inference_script,
    const std::string& tie_break,
    uint32_t seed
) {
    if (model_black.empty() || model_white.empty()) {
        throw std::invalid_argument("play_duel_once requires model_black and model_white");
    }

    SelfPlay sp;
    sp.play_duel_game(
        model_black,
        model_white,
        inference_script,
        parse_tie_break(tie_break),
        seed
    );
    return sp.result;
}
} // namespace

PYBIND11_MODULE(othello_cpp_engine, m) {
    m.doc() = "pybind11 wrapper for Othello C++ engine";

    py::class_<BitBoard>(m, "BitBoard")
        .def(py::init<>())
        .def(py::init<uint64_t, uint64_t, bool>(),
             py::arg("black"), py::arg("white"), py::arg("black_to_move") = true)
        .def_readwrite("black", &BitBoard::black)
        .def_readwrite("white", &BitBoard::white)
        .def_readwrite("black_to_move", &BitBoard::black_to_move)
        .def("get_legal_moves", &BitBoard::get_legal_moves)
        .def("get_legal_move_indices", &legal_move_indices)
        .def("do_move", &BitBoard::do_move, py::arg("pos"))
        .def("is_game_over", &BitBoard::is_game_over)
        .def("count_black", &BitBoard::count_black)
        .def("count_white", &BitBoard::count_white)
        .def("reset", &BitBoard::reset)
        .def("to_array", &BitBoard::to_array);

    m.def(
        "generate_dataset_beam",
        &generate_dataset_beam,
        py::arg("mode"),
        py::arg("dataset_path"),
        py::arg("inference_script") = "ai/python/value_inference_wrapper.py",
        py::arg("model_path") = "",
        py::arg("model_black") = "",
        py::arg("model_white") = "",
        py::arg("model_side") = "black",
        py::arg("beam_width") = 32,
        py::arg("beam_top_k") = 2,
        py::arg("tie_break") = "fixed",
        py::arg("log_interval") = 1,
        py::arg("seed") = 42
    );

    m.def(
        "play_duel_once",
        &play_duel_once,
        py::arg("model_black"),
        py::arg("model_white"),
        py::arg("inference_script") = "ai/python/value_inference_wrapper.py",
        py::arg("tie_break") = "fixed",
        py::arg("seed") = 42
    );
}
