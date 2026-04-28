#include <gtest/gtest.h>

#include <utility>

#include "library.hpp"

namespace {

class ConstructorSizeTest : public ::testing::TestWithParam<int> {};

TEST_P(ConstructorSizeTest, ConstructorSetsSize) {
    const int size = GetParam();
    Array arr(size);
    EXPECT_EQ(arr.size(), size);
}

INSTANTIATE_TEST_SUITE_P(BasicSizes, ConstructorSizeTest, ::testing::Values(0, 1, 8));

struct AccessParam {
    int size;
    int factor;
};

class AccessTest : public ::testing::TestWithParam<AccessParam> {};

TEST_P(AccessTest, ReadWriteThroughOperatorAndAt) {
    const AccessParam param = GetParam();
    Array arr(param.size);

    for (int i = 0; i < param.size; ++i) {
        arr[i] = i * param.factor;
    }

    for (int i = 0; i < param.size; ++i) {
        const int expected = i * param.factor;
        EXPECT_EQ(arr[i], expected);
        EXPECT_EQ(arr.at(i), expected);
    }
}

INSTANTIATE_TEST_SUITE_P(
    MultiplePatterns,
    AccessTest,
    ::testing::Values(
        AccessParam{1, 10},
        AccessParam{5, 3},
        AccessParam{10, -2}));

class DeepCopyTest : public ::testing::TestWithParam<int> {};

TEST_P(DeepCopyTest, CopyConstructorKeepsOriginalIndependent) {
    const int size = GetParam();
    Array source(size);

    for (int i = 0; i < size; ++i) {
        source[i] = i + 100;
    }

    Array copied(source);
    EXPECT_EQ(copied.size(), source.size());

    for (int i = 0; i < size; ++i) {
        EXPECT_EQ(copied[i], source[i]);
    }

    copied[0] = -999;
    EXPECT_EQ(source[0], 100);
}

INSTANTIATE_TEST_SUITE_P(NonEmptySizes, DeepCopyTest, ::testing::Values(1, 5, 10));

struct AllocateParam {
    int initial_size;
    int resized_size;
};

class AllocateClearTest : public ::testing::TestWithParam<AllocateParam> {};

TEST_P(AllocateClearTest, AllocateAndClearChangeSizeAsExpected) {
    const AllocateParam param = GetParam();
    Array arr(param.initial_size);
    EXPECT_EQ(arr.size(), param.initial_size);

    arr.allocate(param.resized_size);
    EXPECT_EQ(arr.size(), param.resized_size);

    if (param.resized_size > 0) {
        arr[0] = 42;
        EXPECT_EQ(arr[0], 42);
    }

    arr.clear();
    EXPECT_EQ(arr.size(), 0);
}

INSTANTIATE_TEST_SUITE_P(
    ResizePatterns,
    AllocateClearTest,
    ::testing::Values(
        AllocateParam{0, 1},
        AllocateParam{4, 0},
        AllocateParam{10, 3}));

TEST(ArrayMoveTest, MoveAssignmentTransfersOwnership) {
    Array source(5);
    for (int i = 0; i < source.size(); ++i) {
        source[i] = i;
    }

    Array target;
    target = std::move(source);

    EXPECT_EQ(target.size(), 5);
    EXPECT_EQ(target[0], 0);
    EXPECT_EQ(target[4], 4);
    EXPECT_EQ(source.size(), 0);
}

#ifndef NDEBUG
TEST(ArrayDeathTest, OutOfRangeAccessTriggersAssert) {
    Array arr(2);
    EXPECT_DEATH(
        {
            const int value = arr.at(2);
            (void)value;
        },
        ".*");
    EXPECT_DEATH(
        {
            arr[-1] = 0;
        },
        ".*");
}
#endif

}  // namespace
