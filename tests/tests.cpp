#include <gtest/gtest.h>

#include "app_error.h"
#include "models.h"
#include "tokenizer.h"

#include <string>
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

namespace {

bool HasErrorType(
    const ErrorSet& errors,
    ErrorType expectedType
) {
    for (const AppError& error : errors.getErrors()) {
        if (error.getType() == expectedType) {
            return true;
        }
    }

    return false;
}

}

// 1. Пустая строка

TEST(TokenizerTest, ReportsEmptyInput) {
    Tokenizer tokenizer;
    ErrorSet errors;

    std::vector<Token> tokens =
        tokenizer.tokenize("", errors);

    EXPECT_TRUE(errors.hasErrors());
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::EmptyInput)
    );

    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, TokenType::End);
}

// 2. Строка только из пробелов

TEST(TokenizerTest, ReportsOnlySpacesInput) {
    Tokenizer tokenizer;
    ErrorSet errors;

    std::vector<Token> tokens =
        tokenizer.tokenize("    ", errors);

    EXPECT_TRUE(errors.hasErrors());
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::OnlySpacesInput)
    );

    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, TokenType::End);
}

// 3. Одно корректное число

TEST(TokenizerTest, TokenizesSingleNumber) {
    Tokenizer tokenizer;
    ErrorSet errors;

    std::vector<Token> tokens =
        tokenizer.tokenize("123", errors);

    EXPECT_FALSE(errors.hasErrors());

    ASSERT_EQ(tokens.size(), 2);

    EXPECT_EQ(tokens[0].type, TokenType::Number);
    EXPECT_EQ(tokens[0].text, "123");
    EXPECT_EQ(tokens[0].value, 123);

    EXPECT_EQ(tokens[1].type, TokenType::End);
}

// 4. Число на нижней недопустимой границе

TEST(TokenizerTest, ReportsZeroOperand) {
    Tokenizer tokenizer;
    ErrorSet errors;

    std::vector<Token> tokens =
        tokenizer.tokenize("0", errors);

    EXPECT_TRUE(errors.hasErrors());
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::ZeroOperand)
    );

    ASSERT_EQ(tokens.size(), 2);

    EXPECT_EQ(tokens[0].type, TokenType::Number);
    EXPECT_EQ(tokens[0].value, 0);
    EXPECT_EQ(tokens[1].type, TokenType::End);
}

// 5. Число на верхней допустимой границе

TEST(TokenizerTest, AcceptsMaximumInputNumber) {
    Tokenizer tokenizer;
    ErrorSet errors;

    std::vector<Token> tokens =
        tokenizer.tokenize("500000", errors);

    EXPECT_FALSE(errors.hasErrors());

    ASSERT_EQ(tokens.size(), 2);

    EXPECT_EQ(tokens[0].type, TokenType::Number);
    EXPECT_EQ(tokens[0].value, 500000);
    EXPECT_EQ(tokens[1].type, TokenType::End);
}

// 6. Число больше допустимого диапазона

TEST(TokenizerTest, ReportsNumberOutOfRange) {
    Tokenizer tokenizer;
    ErrorSet errors;

    std::vector<Token> tokens =
        tokenizer.tokenize("500001", errors);

    EXPECT_TRUE(errors.hasErrors());
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::NumberOutOfRange)
    );

    ASSERT_EQ(tokens.size(), 2);

    EXPECT_EQ(tokens[0].type, TokenType::Number);
    EXPECT_EQ(tokens[0].value, 500001);
    EXPECT_EQ(tokens[1].type, TokenType::End);
}

// 7. Простое сложение

TEST(TokenizerTest, TokenizesSimpleAddition) {
    Tokenizer tokenizer;
    ErrorSet errors;

    std::vector<Token> tokens =
        tokenizer.tokenize("2 + 3", errors);

    EXPECT_FALSE(errors.hasErrors());

    ASSERT_EQ(tokens.size(), 4);

    EXPECT_EQ(tokens[0].type, TokenType::Number);
    EXPECT_EQ(tokens[0].value, 2);

    EXPECT_EQ(tokens[1].type, TokenType::Operation);
    EXPECT_EQ(tokens[1].operation, OperationType::Add);

    EXPECT_EQ(tokens[2].type, TokenType::Number);
    EXPECT_EQ(tokens[2].value, 3);

    EXPECT_EQ(tokens[3].type, TokenType::End);
}

// 8. Простое вычитание

TEST(TokenizerTest, TokenizesSimpleSubtraction) {
    Tokenizer tokenizer;
    ErrorSet errors;

    std::vector<Token> tokens =
        tokenizer.tokenize("5 - 2", errors);

    EXPECT_FALSE(errors.hasErrors());

    ASSERT_EQ(tokens.size(), 4);

    EXPECT_EQ(tokens[0].type, TokenType::Number);
    EXPECT_EQ(tokens[0].value, 5);

    EXPECT_EQ(tokens[1].type, TokenType::Operation);
    EXPECT_EQ(tokens[1].operation, OperationType::Subtract);

    EXPECT_EQ(tokens[2].type, TokenType::Number);
    EXPECT_EQ(tokens[2].value, 2);

    EXPECT_EQ(tokens[3].type, TokenType::End);
}

// 9. Простое умножение

TEST(TokenizerTest, TokenizesSimpleMultiplication) {
    Tokenizer tokenizer;
    ErrorSet errors;

    std::vector<Token> tokens =
        tokenizer.tokenize("4 * 6", errors);

    EXPECT_FALSE(errors.hasErrors());

    ASSERT_EQ(tokens.size(), 4);

    EXPECT_EQ(tokens[0].type, TokenType::Number);
    EXPECT_EQ(tokens[0].value, 4);

    EXPECT_EQ(tokens[1].type, TokenType::Operation);
    EXPECT_EQ(tokens[1].operation, OperationType::Multiply);

    EXPECT_EQ(tokens[2].type, TokenType::Number);
    EXPECT_EQ(tokens[2].value, 6);

    EXPECT_EQ(tokens[3].type, TokenType::End);
}

// 10. Простое деление

TEST(TokenizerTest, TokenizesSimpleDivision) {
    Tokenizer tokenizer;
    ErrorSet errors;

    std::vector<Token> tokens =
        tokenizer.tokenize("8 / 2", errors);

    EXPECT_FALSE(errors.hasErrors());

    ASSERT_EQ(tokens.size(), 4);

    EXPECT_EQ(tokens[0].type, TokenType::Number);
    EXPECT_EQ(tokens[0].value, 8);

    EXPECT_EQ(tokens[1].type, TokenType::Operation);
    EXPECT_EQ(tokens[1].operation, OperationType::Divide);

    EXPECT_EQ(tokens[2].type, TokenType::Number);
    EXPECT_EQ(tokens[2].value, 2);

    EXPECT_EQ(tokens[3].type, TokenType::End);
}

// 11. Выражение со всеми операциями

TEST(TokenizerTest, TokenizesAllSupportedOperations) {
    Tokenizer tokenizer;
    ErrorSet errors;

    std::vector<Token> tokens =
        tokenizer.tokenize(
            "2 + 3 - 4 * 5 / 6",
            errors
        );

    EXPECT_FALSE(errors.hasErrors());

    ASSERT_EQ(tokens.size(), 10);

    EXPECT_EQ(tokens[0].value, 2);
    EXPECT_EQ(tokens[1].operation, OperationType::Add);
    EXPECT_EQ(tokens[2].value, 3);
    EXPECT_EQ(tokens[3].operation, OperationType::Subtract);
    EXPECT_EQ(tokens[4].value, 4);
    EXPECT_EQ(tokens[5].operation, OperationType::Multiply);
    EXPECT_EQ(tokens[6].value, 5);
    EXPECT_EQ(tokens[7].operation, OperationType::Divide);
    EXPECT_EQ(tokens[8].value, 6);
    EXPECT_EQ(tokens[9].type, TokenType::End);
}

// 12. Выражение с открывающей и закрывающей скобкой

TEST(TokenizerTest, TokenizesBrackets) {
    Tokenizer tokenizer;
    ErrorSet errors;

    std::vector<Token> tokens =
        tokenizer.tokenize("(2 + 3)", errors);

    EXPECT_FALSE(errors.hasErrors());

    ASSERT_EQ(tokens.size(), 6);

    EXPECT_EQ(tokens[0].type, TokenType::LeftBracket);

    EXPECT_EQ(tokens[1].type, TokenType::Number);
    EXPECT_EQ(tokens[1].value, 2);

    EXPECT_EQ(tokens[2].type, TokenType::Operation);
    EXPECT_EQ(tokens[2].operation, OperationType::Add);

    EXPECT_EQ(tokens[3].type, TokenType::Number);
    EXPECT_EQ(tokens[3].value, 3);

    EXPECT_EQ(tokens[4].type, TokenType::RightBracket);
    EXPECT_EQ(tokens[5].type, TokenType::End);
}

// 13. Выражение со вложенными скобками

TEST(TokenizerTest, TokenizesNestedBrackets) {
    Tokenizer tokenizer;
    ErrorSet errors;

    std::vector<Token> tokens =
        tokenizer.tokenize(
            "((2 + 3) * 4)",
            errors
        );

    EXPECT_FALSE(errors.hasErrors());

    ASSERT_EQ(tokens.size(), 10);

    EXPECT_EQ(tokens[0].type, TokenType::LeftBracket);
    EXPECT_EQ(tokens[1].type, TokenType::LeftBracket);

    EXPECT_EQ(tokens[2].type, TokenType::Number);
    EXPECT_EQ(tokens[2].value, 2);

    EXPECT_EQ(tokens[3].type, TokenType::Operation);
    EXPECT_EQ(tokens[3].operation, OperationType::Add);

    EXPECT_EQ(tokens[4].type, TokenType::Number);
    EXPECT_EQ(tokens[4].value, 3);

    EXPECT_EQ(tokens[5].type, TokenType::RightBracket);

    EXPECT_EQ(tokens[6].type, TokenType::Operation);
    EXPECT_EQ(tokens[6].operation, OperationType::Multiply);

    EXPECT_EQ(tokens[7].type, TokenType::Number);
    EXPECT_EQ(tokens[7].value, 4);

    EXPECT_EQ(tokens[8].type, TokenType::RightBracket);
    EXPECT_EQ(tokens[9].type, TokenType::End);
}

// 14. Выражение с лишними пробелами

TEST(TokenizerTest, IgnoresExtraSpaces) {
    Tokenizer tokenizer;
    ErrorSet errors;

    std::vector<Token> tokens =
        tokenizer.tokenize(
            "  12  +  5  ",
            errors
        );

    EXPECT_FALSE(errors.hasErrors());

    ASSERT_EQ(tokens.size(), 4);

    EXPECT_EQ(tokens[0].type, TokenType::Number);
    EXPECT_EQ(tokens[0].value, 12);

    EXPECT_EQ(tokens[1].type, TokenType::Operation);
    EXPECT_EQ(tokens[1].operation, OperationType::Add);

    EXPECT_EQ(tokens[2].type, TokenType::Number);
    EXPECT_EQ(tokens[2].value, 5);

    EXPECT_EQ(tokens[3].type, TokenType::End);
}

// 15. Дробное число через точку

TEST(TokenizerTest, ReportsDecimalPoint) {
    Tokenizer tokenizer;
    ErrorSet errors;

    std::vector<Token> tokens =
        tokenizer.tokenize("2.5 + 3", errors);

    EXPECT_TRUE(errors.hasErrors());
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::NotIntegerNumber)
    );

    ASSERT_EQ(tokens.size(), 5);

    EXPECT_EQ(tokens[0].type, TokenType::Number);
    EXPECT_EQ(tokens[0].value, 2);

    EXPECT_EQ(tokens[1].type, TokenType::Number);
    EXPECT_EQ(tokens[1].value, 5);

    EXPECT_EQ(tokens[2].type, TokenType::Operation);
    EXPECT_EQ(tokens[2].operation, OperationType::Add);

    EXPECT_EQ(tokens[3].type, TokenType::Number);
    EXPECT_EQ(tokens[3].value, 3);

    EXPECT_EQ(tokens[4].type, TokenType::End);
}

// 16. Дробное число через запятую

TEST(TokenizerTest, ReportsDecimalComma) {
    Tokenizer tokenizer;
    ErrorSet errors;

    std::vector<Token> tokens =
        tokenizer.tokenize("2,5 + 3", errors);

    EXPECT_TRUE(errors.hasErrors());
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::NotIntegerNumber)
    );

    ASSERT_EQ(tokens.size(), 5);

    EXPECT_EQ(tokens[0].type, TokenType::Number);
    EXPECT_EQ(tokens[0].value, 2);

    EXPECT_EQ(tokens[1].type, TokenType::Number);
    EXPECT_EQ(tokens[1].value, 5);

    EXPECT_EQ(tokens[2].type, TokenType::Operation);
    EXPECT_EQ(tokens[2].operation, OperationType::Add);

    EXPECT_EQ(tokens[3].type, TokenType::Number);
    EXPECT_EQ(tokens[3].value, 3);

    EXPECT_EQ(tokens[4].type, TokenType::End);
}

// 17. Недопустимый символ

TEST(TokenizerTest, ReportsInvalidLatinSymbol) {
    Tokenizer tokenizer;
    ErrorSet errors;

    std::vector<Token> tokens =
        tokenizer.tokenize("2 + a", errors);

    EXPECT_TRUE(errors.hasErrors());
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::InvalidSymbol)
    );

    ASSERT_EQ(tokens.size(), 3);

    EXPECT_EQ(tokens[0].type, TokenType::Number);
    EXPECT_EQ(tokens[0].value, 2);

    EXPECT_EQ(tokens[1].type, TokenType::Operation);
    EXPECT_EQ(tokens[1].operation, OperationType::Add);

    EXPECT_EQ(tokens[2].type, TokenType::End);
}

// 18. Кириллическая буква

TEST(TokenizerTest, ReportsInvalidCyrillicSymbol) {
    Tokenizer tokenizer;
    ErrorSet errors;

    std::vector<Token> tokens =
        tokenizer.tokenize("2 + б", errors);

    EXPECT_TRUE(errors.hasErrors());
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::InvalidSymbol)
    );

    ASSERT_EQ(tokens.size(), 3);

    EXPECT_EQ(tokens[0].type, TokenType::Number);
    EXPECT_EQ(tokens[0].value, 2);

    EXPECT_EQ(tokens[1].type, TokenType::Operation);
    EXPECT_EQ(tokens[1].operation, OperationType::Add);

    EXPECT_EQ(tokens[2].type, TokenType::End);
}

// 19. Неподдерживаемая операция

TEST(TokenizerTest, ReportsUnsupportedOperation) {
    Tokenizer tokenizer;
    ErrorSet errors;

    std::vector<Token> tokens =
        tokenizer.tokenize("2 % 3", errors);

    EXPECT_TRUE(errors.hasErrors());
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::InvalidSymbol)
    );

    ASSERT_EQ(tokens.size(), 3);

    EXPECT_EQ(tokens[0].type, TokenType::Number);
    EXPECT_EQ(tokens[0].value, 2);

    EXPECT_EQ(tokens[1].type, TokenType::Number);
    EXPECT_EQ(tokens[1].value, 3);

    EXPECT_EQ(tokens[2].type, TokenType::End);
}

// 20. Выражение начинается с минуса

TEST(TokenizerTest, TokenizesExpressionStartingWithMinus) {
    Tokenizer tokenizer;
    ErrorSet errors;

    std::vector<Token> tokens =
        tokenizer.tokenize("-2 + 3", errors);

    EXPECT_FALSE(errors.hasErrors());

    ASSERT_EQ(tokens.size(), 5);

    EXPECT_EQ(tokens[0].type, TokenType::Operation);
    EXPECT_EQ(tokens[0].operation, OperationType::Subtract);

    EXPECT_EQ(tokens[1].type, TokenType::Number);
    EXPECT_EQ(tokens[1].value, 2);

    EXPECT_EQ(tokens[2].type, TokenType::Operation);
    EXPECT_EQ(tokens[2].operation, OperationType::Add);

    EXPECT_EQ(tokens[3].type, TokenType::Number);
    EXPECT_EQ(tokens[3].value, 3);

    EXPECT_EQ(tokens[4].type, TokenType::End);
}

// 21. Два числа подряд через пробел

TEST(TokenizerTest, KeepsConsecutiveNumbersAsSeparateTokens) {
    Tokenizer tokenizer;
    ErrorSet errors;

    std::vector<Token> tokens =
        tokenizer.tokenize("1 2", errors);

    EXPECT_FALSE(errors.hasErrors());

    ASSERT_EQ(tokens.size(), 3);

    EXPECT_EQ(tokens[0].type, TokenType::Number);
    EXPECT_EQ(tokens[0].value, 1);

    EXPECT_EQ(tokens[1].type, TokenType::Number);
    EXPECT_EQ(tokens[1].value, 2);

    EXPECT_EQ(tokens[2].type, TokenType::End);
}

// 22. Выражение заканчивается операцией

TEST(TokenizerTest, TokenizesExpressionEndingWithOperation) {
    Tokenizer tokenizer;
    ErrorSet errors;

    std::vector<Token> tokens =
        tokenizer.tokenize("2 + 3 -", errors);

    EXPECT_FALSE(errors.hasErrors());

    ASSERT_EQ(tokens.size(), 5);

    EXPECT_EQ(tokens[0].type, TokenType::Number);
    EXPECT_EQ(tokens[0].value, 2);

    EXPECT_EQ(tokens[1].type, TokenType::Operation);
    EXPECT_EQ(tokens[1].operation, OperationType::Add);

    EXPECT_EQ(tokens[2].type, TokenType::Number);
    EXPECT_EQ(tokens[2].value, 3);

    EXPECT_EQ(tokens[3].type, TokenType::Operation);
    EXPECT_EQ(tokens[3].operation, OperationType::Subtract);

    EXPECT_EQ(tokens[4].type, TokenType::End);
}

// 23. Две операции подряд

TEST(TokenizerTest, KeepsConsecutiveOperationsAsSeparateTokens) {
    Tokenizer tokenizer;
    ErrorSet errors;

    std::vector<Token> tokens =
        tokenizer.tokenize("2 + + 3", errors);

    EXPECT_FALSE(errors.hasErrors());

    ASSERT_EQ(tokens.size(), 5);

    EXPECT_EQ(tokens[0].type, TokenType::Number);
    EXPECT_EQ(tokens[0].value, 2);

    EXPECT_EQ(tokens[1].type, TokenType::Operation);
    EXPECT_EQ(tokens[1].operation, OperationType::Add);

    EXPECT_EQ(tokens[2].type, TokenType::Operation);
    EXPECT_EQ(tokens[2].operation, OperationType::Add);

    EXPECT_EQ(tokens[3].type, TokenType::Number);
    EXPECT_EQ(tokens[3].value, 3);

    EXPECT_EQ(tokens[4].type, TokenType::End);
}

// 24. Пустые скобки

TEST(TokenizerTest, TokenizesEmptyBrackets) {
    Tokenizer tokenizer;
    ErrorSet errors;

    std::vector<Token> tokens =
        tokenizer.tokenize("2 + ()", errors);

    EXPECT_FALSE(errors.hasErrors());

    ASSERT_EQ(tokens.size(), 5);

    EXPECT_EQ(tokens[0].type, TokenType::Number);
    EXPECT_EQ(tokens[0].value, 2);

    EXPECT_EQ(tokens[1].type, TokenType::Operation);
    EXPECT_EQ(tokens[1].operation, OperationType::Add);

    EXPECT_EQ(tokens[2].type, TokenType::LeftBracket);
    EXPECT_EQ(tokens[3].type, TokenType::RightBracket);
    EXPECT_EQ(tokens[4].type, TokenType::End);
}

// 25. Выражение длиной больше допустимой

TEST(TokenizerTest, ReportsExpressionTooLong) {
    Tokenizer tokenizer;
    ErrorSet errors;

    std::string expression;

    for (int i = 0; i < 51; ++i) {
        expression += "1+";
    }

    expression += "1";

    ASSERT_GT(expression.size(), 100);

    std::vector<Token> tokens =
        tokenizer.tokenize(expression, errors);

    EXPECT_TRUE(errors.hasErrors());
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::ExpressionTooLong)
    );

    ASSERT_FALSE(tokens.empty());
    EXPECT_EQ(tokens.back().type, TokenType::End);
}