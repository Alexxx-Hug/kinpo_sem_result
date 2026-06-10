#include <gtest/gtest.h>

#include "models.h"

#include <vector>

TEST(CalculationStepTest, CreatesAdditionStep) {
    CalculationStep step{
        2,
        3,
        OperationType::Add,
        5
    };

    EXPECT_EQ(step.left, 2);
    EXPECT_EQ(step.right, 3);
    EXPECT_EQ(step.operation, OperationType::Add);
    EXPECT_EQ(step.result, 5);
}

TEST(CalculationStepTest, CreatesSubtractionStep) {
    CalculationStep step{
        7,
        3,
        OperationType::Subtract,
        4
    };

    EXPECT_EQ(step.left, 7);
    EXPECT_EQ(step.right, 3);
    EXPECT_EQ(step.operation, OperationType::Subtract);
    EXPECT_EQ(step.result, 4);
}

TEST(CalculationStepTest, CreatesMultiplicationStep) {
    CalculationStep step{
        4,
        6,
        OperationType::Multiply,
        24
    };

    EXPECT_EQ(step.left, 4);
    EXPECT_EQ(step.right, 6);
    EXPECT_EQ(step.operation, OperationType::Multiply);
    EXPECT_EQ(step.result, 24);
}

TEST(CalculationStepTest, CreatesDivisionStep) {
    CalculationStep step{
        20,
        5,
        OperationType::Divide,
        4
    };

    EXPECT_EQ(step.left, 20);
    EXPECT_EQ(step.right, 5);
    EXPECT_EQ(step.operation, OperationType::Divide);
    EXPECT_EQ(step.result, 4);
}

TEST(CalculationStepTest, StoresZeroResult) {
    CalculationStep step{
        5,
        5,
        OperationType::Subtract,
        0
    };

    EXPECT_EQ(step.left, 5);
    EXPECT_EQ(step.right, 5);
    EXPECT_EQ(step.operation, OperationType::Subtract);
    EXPECT_EQ(step.result, 0);
}

TEST(CalculationStepTest, StoresLargeResult) {
    CalculationStep step{
        500000,
        2,
        OperationType::Multiply,
        1000000
    };

    EXPECT_EQ(step.left, 500000);
    EXPECT_EQ(step.right, 2);
    EXPECT_EQ(step.operation, OperationType::Multiply);
    EXPECT_EQ(step.result, 1000000);
}

TEST(CalculationStepTest, AddsStepToVector) {
    std::vector<CalculationStep> steps;

    steps.emplace_back(
        2,
        5,
        OperationType::Add,
        7
    );

    ASSERT_EQ(steps.size(), 1);

    EXPECT_EQ(steps[0].left, 2);
    EXPECT_EQ(steps[0].right, 5);
    EXPECT_EQ(steps[0].operation, OperationType::Add);
    EXPECT_EQ(steps[0].result, 7);
}

TEST(CalculationStepTest, PreservesStepsOrder) {
    std::vector<CalculationStep> steps;

    steps.emplace_back(
        2,
        5,
        OperationType::Add,
        7
    );

    steps.emplace_back(
        7,
        3,
        OperationType::Subtract,
        4
    );

    ASSERT_EQ(steps.size(), 2);

    EXPECT_EQ(steps[0].operation, OperationType::Add);
    EXPECT_EQ(steps[0].result, 7);

    EXPECT_EQ(steps[1].operation, OperationType::Subtract);
    EXPECT_EQ(steps[1].result, 4);
}

TEST(CalculationStepTest, ClearsStepsVector) {
    std::vector<CalculationStep> steps;

    steps.emplace_back(
        2,
        3,
        OperationType::Add,
        5
    );

    ASSERT_EQ(steps.size(), 1);

    steps.clear();

    EXPECT_TRUE(steps.empty());
    EXPECT_EQ(steps.size(), 0);
}

