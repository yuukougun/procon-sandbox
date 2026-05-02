#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "library/cpp/sample_math.hpp"
#include "library/cpp/sample_math.cpp"

namespace {

struct SumParam {
    std::vector<int> values;
    int expected;
};

class SampleMathSumTest : public ::testing::TestWithParam<SumParam> {};

TEST_P(SampleMathSumTest, ReturnsExpectedTotal) {
    const SumParam param = GetParam();
    EXPECT_EQ(sample::SampleMath::sum(param.values), param.expected);
}

INSTANTIATE_TEST_SUITE_P(
    TypicalInputs,
    SampleMathSumTest,
    ::testing::Values(
        SumParam{{}, 0},
        SumParam{{1}, 1},
        SumParam{{1, 2, 3, 4}, 10},
        SumParam{{-5, 2, 10}, 7}));

struct GreetParam {
    std::string name;
    std::string expected;
};

class SampleMathGreetTest : public ::testing::TestWithParam<GreetParam> {};

TEST_P(SampleMathGreetTest, ReturnsExpectedMessage) {
    const GreetParam param = GetParam();
    EXPECT_EQ(sample::SampleMath::greet(param.name), param.expected);
}

INSTANTIATE_TEST_SUITE_P(
    TypicalNames,
    SampleMathGreetTest,
    ::testing::Values(
        GreetParam{"", "Hi, my name is yuki"},
        GreetParam{"Alice", "Hi, my name is yukiAlice"},
        GreetParam{"Bob", "Hi, my name is yukiBob"}));

}  // namespace
