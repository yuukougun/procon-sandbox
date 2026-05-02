# Copilot Instructions for `procon-sandbox`

## Build, test, and lint commands

Use the repository `Makefile` test targets as the canonical entry points (CI also calls these):

- Full regression suite (C++ + Python): `make test`
- C++ tests only: `make test-cpp`
- Python tests only: `make test-py`
- Python test wrapper (runs both targets in sequence): `python3 tester/testAll.py`

Run a single test:

- Python (single test function):  
  `PYTHONPATH=. pytest -q tester/python/unit/library/test_array.py::test_constructor_and_size`
- C++ (single gtest suite after build):  
  `cmake -S tester/cpp -B tester/cpp/build && cmake --build tester/cpp/build && ctest --test-dir tester/cpp/build -R ConstructorSizeTest --output-on-failure`

Docs pipeline commands used by CI:

- Detect whether docs build should run for a change set:  
  `python .github/scripts/detect_library_changes.py --config .github/libraries.json --event-name <event> --before-sha <sha> --current-sha <sha>`
- Build docs site locally (Doxygen + Sphinx + generated index):  
  `python .github/scripts/build_docs_site.py --config .github/libraries.json --output site`

Project-specific build/dev commands:

- Build pybind11 extension used by AI Python modules:  
  `source /home/dev/.venv/bin/activate && python ai/python/build_cpp_engine.py build_ext`
- Run the Yew web visualizer:  
  `cd ai/visualizer/yew-othello && trunk serve --open`

No repository-wide lint command is currently defined in `Makefile` or workflows.

## High-level architecture

- This repository is a multi-language development sandbox/template (C++, Python, Rust) with shared infrastructure in one repo.
- `library/` contains shared sample libraries (C++ and Python). `tester/` is the regression harness validating those APIs in both languages.
- CI test workflow (`.github/workflows/test.yml`) runs `make test-cpp` and `make test-py` as separate jobs, then aggregates status.
- Documentation publishing is config-driven:
  - `.github/libraries.json` declares doc targets (language, source paths, output subdir, enable flag).
  - `.github/scripts/detect_library_changes.py` gates whether docs build runs.
  - `.github/scripts/build_docs_site.py` generates Doxygen/Sphinx output and an index page, then Pages deploy workflow publishes it.
- AI stack (`ai/`) bridges C++ engine + Python training/inference:
  - `ai/cpp` provides bitboard/self-play core and pybind module (`othello_cpp_engine`).
  - `ai/python` consumes the extension for dataset generation/training/inference services (`engine.py`, `dataset.py`, `model_move_server.py`).
  - `ai/visualizer/yew-othello` is a Rust/Yew frontend that calls the Python move server over HTTP.
- Rust workspace root (`Cargo.toml`) currently includes `ai/visualizer/yew-othello` as a member; `visualizer/` is a separate Rust crate prototype.

## Key conventions in this codebase

- Keep test entrypoints aligned with CI: if you add or change tests, wire them through existing `make` targets instead of introducing ad-hoc commands.
- C++ tests under `tester/cpp/unit/*_test.cpp` are discovered via CMake glob + `gtest_discover_tests`; tests are expected to be parameterized where practical.
- Python tests under `tester/python/` rely on `tester/python/conftest.py` for import path setup (repo root + optional `TESTER_EXTRA_PYTHONPATH`).
- In `tester/cpp/CMakeLists.txt`, test binaries compile test sources only; when implementation symbols are needed, tests may include `.cpp` directly (see `sample_math_test.cpp`).
- Docs are generated from `docs/_navigation.md` and auto-synchronized:
  - Git hook path is set to `docs/git-hooks`.
  - When staged changes include `docs/`, pre-commit runs `docs/script/gen-contribut` and re-generates `CONTRIBUTING.md` plus docs navigation blocks.
- Naming rules from project docs: snake_case for files/functions/variables, PascalCase for classes/types, SCREAMING_CASE for constants, boolean names prefixed with `is_`.
