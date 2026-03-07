CXX = g++
CXXFLAGS = -std=c++20 -O2 -Wall -Wextra
SOLVE_DIR = solve
BUILD_DIR = build

SOLVE_SRC = $(SOLVE_DIR)/solve.cpp
SOLVE_BIN = $(BUILD_DIR)/solve

.PHONY: all clean run test

all: $(SOLVE_BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(SOLVE_BIN): $(SOLVE_SRC) $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -rf $(BUILD_DIR)

run: $(SOLVE_BIN)
	./$(SOLVE_BIN)

test:
	python3 tester/testAll.py
