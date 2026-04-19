.PHONY: test-cpp test-py test

TEST_CPP_BUILD_DIR := tester/cpp/build

test-cpp:
	cmake -S tester/cpp -B $(TEST_CPP_BUILD_DIR)
	cmake --build $(TEST_CPP_BUILD_DIR)
	ctest --test-dir $(TEST_CPP_BUILD_DIR) --output-on-failure

test-py:
	PYTHONPATH=. pytest -q tester/python

test: test-cpp test-py
