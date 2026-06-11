#include <gtest/gtest.h>

#include "app_error.h"
#include "models.h"
#include "tokenizer.h"
#include "parser.h"
#include "expression_solver.h"

#include <memory>
#include <vector>

#include <string>

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

// Вспомогательные функции для создания токенов Parser

namespace {

Token MakeNumberToken(long long value) {
    return Token(
        TokenType::Number,
        std::to_string(value),
        value
    );
}

Token MakeOperationToken(
    OperationType operation,
    const std::string& text
) {
    return Token(
        TokenType::Operation,
        text,
        operation
    );
}

Token MakeLeftBracketToken() {
    return Token(TokenType::LeftBracket, "(");
}

Token MakeRightBracketToken() {
    return Token(TokenType::RightBracket, ")");
}

Token MakeEndToken() {
    return Token(TokenType::End, "");
}

} // namespace

// 1. Корректное выражение из одного числа

TEST(ParserValidationTest, AcceptsSingleNumber) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeNumberToken(5),
        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_FALSE(errors.hasErrors());
    ASSERT_NE(root, nullptr);

    EXPECT_EQ(root->type, NodeType::Number);
    EXPECT_EQ(root->value, 5);
}


// 2. Корректное сложение


TEST(ParserValidationTest, AcceptsCorrectAddition) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeNumberToken(2),
        MakeOperationToken(OperationType::Add, "+"),
        MakeNumberToken(3),
        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_NE(root, nullptr);
}


// 3. Корректное вычитание


TEST(ParserValidationTest, AcceptsCorrectSubtraction) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeNumberToken(5),
        MakeOperationToken(OperationType::Subtract, "-"),
        MakeNumberToken(2),
        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_NE(root, nullptr);
}


// 4. Корректное умножение


TEST(ParserValidationTest, AcceptsCorrectMultiplication) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeNumberToken(4),
        MakeOperationToken(OperationType::Multiply, "*"),
        MakeNumberToken(6),
        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_NE(root, nullptr);
}


// 5. Корректное деление


TEST(ParserValidationTest, AcceptsCorrectDivision) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeNumberToken(8),
        MakeOperationToken(OperationType::Divide, "/"),
        MakeNumberToken(2),
        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_NE(root, nullptr);
}


// 6. Корректное выражение со скобками


TEST(ParserValidationTest, AcceptsCorrectBracketExpression) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeLeftBracketToken(),
        MakeNumberToken(2),
        MakeOperationToken(OperationType::Add, "+"),
        MakeNumberToken(3),
        MakeRightBracketToken(),
        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_NE(root, nullptr);
}


// 7. Корректное выражение с вложенными скобками


TEST(ParserValidationTest, AcceptsNestedBrackets) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeLeftBracketToken(),
        MakeLeftBracketToken(),
        MakeNumberToken(2),
        MakeOperationToken(OperationType::Add, "+"),
        MakeNumberToken(3),
        MakeRightBracketToken(),
        MakeOperationToken(OperationType::Multiply, "*"),
        MakeNumberToken(4),
        MakeRightBracketToken(),
        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_NE(root, nullptr);
}


// 8. Выражение начинается с операции сложения


TEST(ParserValidationTest, RejectsExpressionStartingWithAddition) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeOperationToken(OperationType::Add, "+"),
        MakeNumberToken(2),
        MakeOperationToken(OperationType::Add, "+"),
        MakeNumberToken(3),
        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_EQ(root, nullptr);
    EXPECT_TRUE(errors.hasErrors());
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::SyntaxError)
    );
}


// 9. Выражение начинается с операции умножения


TEST(ParserValidationTest, RejectsExpressionStartingWithMultiplication) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeOperationToken(OperationType::Multiply, "*"),
        MakeNumberToken(2),
        MakeOperationToken(OperationType::Add, "+"),
        MakeNumberToken(3),
        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_EQ(root, nullptr);
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::SyntaxError)
    );
}


// 10. Выражение начинается с операции деления


TEST(ParserValidationTest, RejectsExpressionStartingWithDivision) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeOperationToken(OperationType::Divide, "/"),
        MakeNumberToken(2),
        MakeOperationToken(OperationType::Add, "+"),
        MakeNumberToken(3),
        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_EQ(root, nullptr);
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::SyntaxError)
    );
}


// 11. Выражение начинается с минуса


TEST(ParserValidationTest, RejectsExpressionStartingWithMinus) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeOperationToken(OperationType::Subtract, "-"),
        MakeNumberToken(2),
        MakeOperationToken(OperationType::Add, "+"),
        MakeNumberToken(3),
        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_EQ(root, nullptr);
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::NegativeInputNumber)
    );
}


// 12. Выражение заканчивается операцией сложения


TEST(ParserValidationTest, RejectsExpressionEndingWithAddition) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeNumberToken(2),
        MakeOperationToken(OperationType::Add, "+"),
        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_EQ(root, nullptr);
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::MissingOperand)
    );
}


// 13. Выражение заканчивается операцией вычитания


TEST(ParserValidationTest, RejectsExpressionEndingWithSubtraction) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeNumberToken(2),
        MakeOperationToken(OperationType::Subtract, "-"),
        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_EQ(root, nullptr);
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::MissingOperand)
    );
}


// 14. Выражение заканчивается операцией умножения


TEST(ParserValidationTest, RejectsExpressionEndingWithMultiplication) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeNumberToken(2),
        MakeOperationToken(OperationType::Multiply, "*"),
        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_EQ(root, nullptr);
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::MissingOperand)
    );
}


// 15. Выражение заканчивается операцией деления


TEST(ParserValidationTest, RejectsExpressionEndingWithDivision) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeNumberToken(2),
        MakeOperationToken(OperationType::Divide, "/"),
        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_EQ(root, nullptr);
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::MissingOperand)
    );
}


// 16. Две операции сложения подряд


TEST(ParserValidationTest, RejectsTwoAdditionsInRow) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeNumberToken(2),
        MakeOperationToken(OperationType::Add, "+"),
        MakeOperationToken(OperationType::Add, "+"),
        MakeNumberToken(3),
        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_EQ(root, nullptr);
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::SyntaxError)
    );
}


// 17. Две разные операции подряд


TEST(ParserValidationTest, RejectsDifferentOperationsInRow) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeNumberToken(2),
        MakeOperationToken(OperationType::Multiply, "*"),
        MakeOperationToken(OperationType::Divide, "/"),
        MakeNumberToken(3),
        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_EQ(root, nullptr);
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::SyntaxError)
    );
}


// 18. Операция после открывающей скобки


TEST(ParserValidationTest, RejectsOperationAfterOpeningBracket) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeLeftBracketToken(),
        MakeOperationToken(OperationType::Multiply, "*"),
        MakeNumberToken(2),
        MakeOperationToken(OperationType::Add, "+"),
        MakeNumberToken(3),
        MakeRightBracketToken(),
        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_EQ(root, nullptr);
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::MissingOperand)
    );
}


// 19. Операция перед закрывающей скобкой


TEST(ParserValidationTest, RejectsOperationBeforeClosingBracket) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeLeftBracketToken(),
        MakeNumberToken(2),
        MakeOperationToken(OperationType::Add, "+"),
        MakeRightBracketToken(),
        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_EQ(root, nullptr);
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::MissingOperand)
    );
}


// 20. Пустые скобки


TEST(ParserValidationTest, RejectsEmptyBrackets) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeNumberToken(2),
        MakeOperationToken(OperationType::Add, "+"),
        MakeLeftBracketToken(),
        MakeRightBracketToken(),
        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_EQ(root, nullptr);
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::EmptyBrackets)
    );
}


// 21. Нет закрывающей скобки


TEST(ParserValidationTest, RejectsMissingClosingBracket) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeLeftBracketToken(),
        MakeNumberToken(2),
        MakeOperationToken(OperationType::Add, "+"),
        MakeNumberToken(3),
        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_EQ(root, nullptr);
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::BracketBalanceError)
    );
}


// 22. Лишняя закрывающая скобка


TEST(ParserValidationTest, RejectsExtraClosingBracket) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeNumberToken(2),
        MakeOperationToken(OperationType::Add, "+"),
        MakeNumberToken(3),
        MakeRightBracketToken(),
        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_EQ(root, nullptr);
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::BracketBalanceError)
    );
}


// 23. Закрывающая скобка раньше открывающей


TEST(ParserValidationTest, RejectsClosingBracketBeforeOpeningBracket) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeRightBracketToken(),
        MakeNumberToken(2),
        MakeOperationToken(OperationType::Add, "+"),
        MakeNumberToken(3),
        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_EQ(root, nullptr);
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::BracketBalanceError)
    );
}


// 24. Число перед открывающей скобкой без операции


TEST(ParserValidationTest, RejectsNumberBeforeOpeningBracket) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeNumberToken(2),
        MakeLeftBracketToken(),
        MakeNumberToken(3),
        MakeOperationToken(OperationType::Add, "+"),
        MakeNumberToken(4),
        MakeRightBracketToken(),
        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_EQ(root, nullptr);
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::MissingOperation)
    );
}


// 25. Закрывающая скобка перед числом без операции


TEST(ParserValidationTest, RejectsNumberAfterClosingBracket) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeLeftBracketToken(),
        MakeNumberToken(2),
        MakeOperationToken(OperationType::Add, "+"),
        MakeNumberToken(3),
        MakeRightBracketToken(),
        MakeNumberToken(4),
        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_EQ(root, nullptr);
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::MissingOperation)
    );
}


// 26. Два скобочных выражения подряд


TEST(ParserValidationTest, RejectsAdjacentBracketExpressions) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeLeftBracketToken(),
        MakeNumberToken(2),
        MakeOperationToken(OperationType::Add, "+"),
        MakeNumberToken(3),
        MakeRightBracketToken(),

        MakeLeftBracketToken(),
        MakeNumberToken(4),
        MakeOperationToken(OperationType::Add, "+"),
        MakeNumberToken(5),
        MakeRightBracketToken(),

        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_EQ(root, nullptr);
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::MissingOperation)
    );
}


// 27. Открывающая скобка после закрывающей без операции


TEST(ParserValidationTest, RejectsOpeningBracketAfterClosingBracket) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeLeftBracketToken(),
        MakeNumberToken(2),
        MakeRightBracketToken(),

        MakeLeftBracketToken(),
        MakeNumberToken(3),
        MakeRightBracketToken(),

        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_EQ(root, nullptr);
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::MissingOperation)
    );
}


// 28. Сложное корректное выражение


TEST(ParserValidationTest, AcceptsComplexCorrectExpression) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeLeftBracketToken(),
        MakeNumberToken(2),
        MakeOperationToken(OperationType::Add, "+"),
        MakeNumberToken(3),
        MakeRightBracketToken(),

        MakeOperationToken(OperationType::Multiply, "*"),

        MakeLeftBracketToken(),
        MakeNumberToken(4),
        MakeOperationToken(OperationType::Subtract, "-"),
        MakeNumberToken(1),
        MakeRightBracketToken(),

        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_NE(root, nullptr);
}

// 29. Несколько синтаксических ошибок

TEST(ParserValidationTest, ReportsMultipleSyntaxErrors) {
    Parser parser;
    ErrorSet errors;

    std::vector<Token> tokens = {
        MakeNumberToken(2),
        MakeOperationToken(OperationType::Add, "+"),
        MakeOperationToken(OperationType::Multiply, "*"),
        MakeRightBracketToken(),
        MakeEndToken()
    };

    ExpressionNode* root = parser.parse(tokens, errors);

    EXPECT_EQ(root, nullptr);
    EXPECT_TRUE(errors.hasErrors());

    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::SyntaxError)
    );

    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::BracketBalanceError)
    );
}

// Вспомогательный класс для создания тестовых деревьев

namespace {

class TestTree {
public:
    ExpressionNode* number(long long value) {
        nodes.push_back(
            std::make_unique<ExpressionNode>(value)
        );

        return nodes.back().get();
    }

    ExpressionNode* operation(
        OperationType operationType,
        ExpressionNode* left,
        ExpressionNode* right
    ) {
        nodes.push_back(
            std::make_unique<ExpressionNode>(
                operationType,
                left,
                right
            )
        );

        return nodes.back().get();
    }

private:
    std::vector<std::unique_ptr<ExpressionNode>> nodes;
};

}

// Тесты ExpressionSolver::solve()

// 1. Пустое дерево

TEST(ExpressionSolverTest, ReportsErrorForNullRoot) {
    ExpressionSolver solver;
    ErrorSet errors;

    SolveResult result = solver.solve(nullptr, errors);

    EXPECT_EQ(result.finalResult, 0);
    EXPECT_TRUE(result.steps.empty());

    EXPECT_TRUE(errors.hasErrors());
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::SyntaxError)
    );
}

// 2. Дерево из одного числа

TEST(ExpressionSolverTest, SolvesSingleNumberTree) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.number(5);

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 5);
    EXPECT_TRUE(result.steps.empty());
}

// 3. Простое сложение

TEST(ExpressionSolverTest, SolvesSimpleAddition) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::Add,
        tree.number(2),
        tree.number(3)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 5);

    ASSERT_EQ(result.steps.size(), 1);

    EXPECT_EQ(result.steps[0].left, 2);
    EXPECT_EQ(result.steps[0].right, 3);
    EXPECT_EQ(
        result.steps[0].operation,
        OperationType::Add
    );
    EXPECT_EQ(result.steps[0].result, 5);
}

// 4. Простое вычитание

TEST(ExpressionSolverTest, SolvesSimpleSubtraction) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::Subtract,
        tree.number(5),
        tree.number(2)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 3);

    ASSERT_EQ(result.steps.size(), 1);

    EXPECT_EQ(result.steps[0].left, 5);
    EXPECT_EQ(result.steps[0].right, 2);
    EXPECT_EQ(
        result.steps[0].operation,
        OperationType::Subtract
    );
    EXPECT_EQ(result.steps[0].result, 3);
}

// 5. Простое умножение

TEST(ExpressionSolverTest, SolvesSimpleMultiplication) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::Multiply,
        tree.number(4),
        tree.number(6)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 24);

    ASSERT_EQ(result.steps.size(), 1);

    EXPECT_EQ(result.steps[0].left, 4);
    EXPECT_EQ(result.steps[0].right, 6);
    EXPECT_EQ(
        result.steps[0].operation,
        OperationType::Multiply
    );
    EXPECT_EQ(result.steps[0].result, 24);
}

// 6. Простое деление

TEST(ExpressionSolverTest, SolvesSimpleDivision) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::Divide,
        tree.number(8),
        tree.number(2)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 4);

    ASSERT_EQ(result.steps.size(), 1);

    EXPECT_EQ(result.steps[0].left, 8);
    EXPECT_EQ(result.steps[0].right, 2);
    EXPECT_EQ(
        result.steps[0].operation,
        OperationType::Divide
    );
    EXPECT_EQ(result.steps[0].result, 4);
}

// 7. Выражение с несколькими сложениями

TEST(ExpressionSolverTest, SolvesMultipleAdditions) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* firstAddition = tree.operation(
        OperationType::Add,
        tree.number(2),
        tree.number(3)
    );

    ExpressionNode* root = tree.operation(
        OperationType::Add,
        firstAddition,
        tree.number(4)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 9);

    ASSERT_EQ(result.steps.size(), 2);

    EXPECT_EQ(result.steps[0].left, 2);
    EXPECT_EQ(result.steps[0].right, 3);
    EXPECT_EQ(result.steps[0].result, 5);

    EXPECT_EQ(result.steps[1].left, 5);
    EXPECT_EQ(result.steps[1].right, 4);
    EXPECT_EQ(result.steps[1].result, 9);
}

// 8. Выражение с несколькими вычитаниями

TEST(ExpressionSolverTest, SolvesMultipleSubtractions) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* firstSubtraction = tree.operation(
        OperationType::Subtract,
        tree.number(10),
        tree.number(3)
    );

    ExpressionNode* root = tree.operation(
        OperationType::Subtract,
        firstSubtraction,
        tree.number(2)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 5);

    ASSERT_EQ(result.steps.size(), 2);

    EXPECT_EQ(result.steps[0].left, 10);
    EXPECT_EQ(result.steps[0].right, 3);
    EXPECT_EQ(result.steps[0].result, 7);

    EXPECT_EQ(result.steps[1].left, 7);
    EXPECT_EQ(result.steps[1].right, 2);
    EXPECT_EQ(result.steps[1].result, 5);
}

// 9. Безопасный порядок сложения и вычитания

TEST(ExpressionSolverTest, SolvesSafeAdditionSubtractionTree) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* addition = tree.operation(
        OperationType::Add,
        tree.number(2),
        tree.number(5)
    );

    ExpressionNode* root = tree.operation(
        OperationType::Subtract,
        addition,
        tree.number(3)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 4);

    ASSERT_EQ(result.steps.size(), 2);

    EXPECT_EQ(result.steps[0].left, 2);
    EXPECT_EQ(result.steps[0].right, 5);
    EXPECT_EQ(
        result.steps[0].operation,
        OperationType::Add
    );
    EXPECT_EQ(result.steps[0].result, 7);

    EXPECT_EQ(result.steps[1].left, 7);
    EXPECT_EQ(result.steps[1].right, 3);
    EXPECT_EQ(
        result.steps[1].operation,
        OperationType::Subtract
    );
    EXPECT_EQ(result.steps[1].result, 4);
}

// 10. Вычитание даёт отрицательный результат

TEST(ExpressionSolverTest, ReportsNegativeSubtractionResult) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::Subtract,
        tree.number(2),
        tree.number(5)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_EQ(result.finalResult, 0);
    EXPECT_TRUE(result.steps.empty());

    EXPECT_TRUE(errors.hasErrors());
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::NegativeResult)
    );
}

// 11. Ошибка после нескольких успешных шагов

TEST(ExpressionSolverTest, PreservesSuccessfulStepsBeforeError) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* addition = tree.operation(
        OperationType::Add,
        tree.number(2),
        tree.number(3)
    );

    ExpressionNode* firstSubtraction = tree.operation(
        OperationType::Subtract,
        addition,
        tree.number(4)
    );

    ExpressionNode* root = tree.operation(
        OperationType::Subtract,
        firstSubtraction,
        tree.number(9)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_EQ(result.finalResult, 0);

    EXPECT_TRUE(errors.hasErrors());
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::NegativeResult)
    );

    // Ошибочный шаг 1 - 9 не сохраняется.
    ASSERT_EQ(result.steps.size(), 2);

    EXPECT_EQ(result.steps[0].left, 2);
    EXPECT_EQ(result.steps[0].right, 3);
    EXPECT_EQ(result.steps[0].result, 5);

    EXPECT_EQ(result.steps[1].left, 5);
    EXPECT_EQ(result.steps[1].right, 4);
    EXPECT_EQ(result.steps[1].result, 1);
}

// 12. Выражение с несколькими умножениями

TEST(ExpressionSolverTest, SolvesMultipleMultiplications) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* firstMultiplication = tree.operation(
        OperationType::Multiply,
        tree.number(2),
        tree.number(3)
    );

    ExpressionNode* root = tree.operation(
        OperationType::Multiply,
        firstMultiplication,
        tree.number(4)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 24);

    ASSERT_EQ(result.steps.size(), 2);

    EXPECT_EQ(result.steps[0].left, 2);
    EXPECT_EQ(result.steps[0].right, 3);
    EXPECT_EQ(result.steps[0].result, 6);

    EXPECT_EQ(result.steps[1].left, 6);
    EXPECT_EQ(result.steps[1].right, 4);
    EXPECT_EQ(result.steps[1].result, 24);
}

// 13. Выражение с несколькими делениями

TEST(ExpressionSolverTest, SolvesMultipleDivisions) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* firstDivision = tree.operation(
        OperationType::Divide,
        tree.number(100),
        tree.number(5)
    );

    ExpressionNode* root = tree.operation(
        OperationType::Divide,
        firstDivision,
        tree.number(2)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 10);

    ASSERT_EQ(result.steps.size(), 2);

    EXPECT_EQ(result.steps[0].left, 100);
    EXPECT_EQ(result.steps[0].right, 5);
    EXPECT_EQ(result.steps[0].result, 20);

    EXPECT_EQ(result.steps[1].left, 20);
    EXPECT_EQ(result.steps[1].right, 2);
    EXPECT_EQ(result.steps[1].result, 10);
}

// 14. Безопасный порядок умножения и деления

TEST(ExpressionSolverTest, SolvesSafeMultiplicationDivisionTree) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* multiplication = tree.operation(
        OperationType::Multiply,
        tree.number(2),
        tree.number(10)
    );

    ExpressionNode* root = tree.operation(
        OperationType::Divide,
        multiplication,
        tree.number(5)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 4);

    ASSERT_EQ(result.steps.size(), 2);

    EXPECT_EQ(result.steps[0].left, 2);
    EXPECT_EQ(result.steps[0].right, 10);
    EXPECT_EQ(result.steps[0].result, 20);

    EXPECT_EQ(result.steps[1].left, 20);
    EXPECT_EQ(result.steps[1].right, 5);
    EXPECT_EQ(result.steps[1].result, 4);
}

// 15. Деление приводит к дробному результату

TEST(ExpressionSolverTest, ReportsFractionResult) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::Divide,
        tree.number(2),
        tree.number(5)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_EQ(result.finalResult, 0);
    EXPECT_TRUE(result.steps.empty());

    EXPECT_TRUE(errors.hasErrors());
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::FractionResult)
    );
}

// 16. Деление на ноль

TEST(ExpressionSolverTest, ReportsDivisionByZero) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::Divide,
        tree.number(8),
        tree.number(0)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_EQ(result.finalResult, 0);
    EXPECT_TRUE(result.steps.empty());

    EXPECT_TRUE(errors.hasErrors());
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::DivisionByZero)
    );
}

// 17. Ошибка деления после успешного шага

TEST(ExpressionSolverTest, KeepsAdditionStepBeforeFractionError) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* addition = tree.operation(
        OperationType::Add,
        tree.number(2),
        tree.number(3)
    );

    ExpressionNode* root = tree.operation(
        OperationType::Divide,
        addition,
        tree.number(2)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_EQ(result.finalResult, 0);

    EXPECT_TRUE(errors.hasErrors());
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::FractionResult)
    );

    ASSERT_EQ(result.steps.size(), 1);

    EXPECT_EQ(result.steps[0].left, 2);
    EXPECT_EQ(result.steps[0].right, 3);
    EXPECT_EQ(result.steps[0].result, 5);
}

// 18. Сложное выражение со скобками

TEST(ExpressionSolverTest, SolvesComplexBracketExpression) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* addition = tree.operation(
        OperationType::Add,
        tree.number(8),
        tree.number(4)
    );

    ExpressionNode* subtraction = tree.operation(
        OperationType::Subtract,
        tree.number(9),
        tree.number(3)
    );

    ExpressionNode* multiplication = tree.operation(
        OperationType::Multiply,
        addition,
        subtraction
    );

    ExpressionNode* root = tree.operation(
        OperationType::Divide,
        multiplication,
        tree.number(6)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 12);

    ASSERT_EQ(result.steps.size(), 4);

    EXPECT_EQ(result.steps[0].result, 12);
    EXPECT_EQ(result.steps[1].result, 6);
    EXPECT_EQ(result.steps[2].result, 72);
    EXPECT_EQ(result.steps[3].result, 12);
}

// 19. Сложное выражение с несколькими поддеревьями

TEST(ExpressionSolverTest, SolvesComplexExpressionWithSubtrees) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* leftAddition = tree.operation(
        OperationType::Add,
        tree.number(8),
        tree.number(4)
    );

    ExpressionNode* leftSubtraction = tree.operation(
        OperationType::Subtract,
        tree.number(9),
        tree.number(3)
    );

    ExpressionNode* multiplication = tree.operation(
        OperationType::Multiply,
        leftAddition,
        leftSubtraction
    );

    ExpressionNode* division = tree.operation(
        OperationType::Divide,
        multiplication,
        tree.number(6)
    );

    ExpressionNode* rightAddition = tree.operation(
        OperationType::Add,
        tree.number(7),
        tree.number(5)
    );

    ExpressionNode* root = tree.operation(
        OperationType::Add,
        division,
        rightAddition
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 24);

    ASSERT_EQ(result.steps.size(), 6);

    EXPECT_EQ(result.steps[0].result, 12);
    EXPECT_EQ(result.steps[1].result, 6);
    EXPECT_EQ(result.steps[2].result, 72);
    EXPECT_EQ(result.steps[3].result, 12);
    EXPECT_EQ(result.steps[4].result, 12);
    EXPECT_EQ(result.steps[5].result, 24);
}

// 20. Переполнение при сложении

TEST(ExpressionSolverTest, ReportsAdditionOverflow) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::Add,
        tree.number(1000000000),
        tree.number(1)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_EQ(result.finalResult, 0);
    EXPECT_TRUE(result.steps.empty());

    EXPECT_TRUE(errors.hasErrors());
    EXPECT_TRUE(
        HasErrorType(
            errors,
            ErrorType::IntermediateOverflow
        )
    );
}

// 21. Переполнение при умножении

TEST(ExpressionSolverTest, ReportsMultiplicationOverflow) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::Multiply,
        tree.number(500000),
        tree.number(500000)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_EQ(result.finalResult, 0);
    EXPECT_TRUE(result.steps.empty());

    EXPECT_TRUE(errors.hasErrors());
    EXPECT_TRUE(
        HasErrorType(
            errors,
            ErrorType::IntermediateOverflow
        )
    );
}

// 22. Выражение с итоговым результатом 0

TEST(ExpressionSolverTest, AllowsZeroSubtractionResult) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::Subtract,
        tree.number(5),
        tree.number(5)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 0);

    ASSERT_EQ(result.steps.size(), 1);

    EXPECT_EQ(result.steps[0].left, 5);
    EXPECT_EQ(result.steps[0].right, 5);
    EXPECT_EQ(result.steps[0].result, 0);
}

// 23. Деление после умножения

TEST(ExpressionSolverTest, SolvesDivisionAfterMultiplication) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* multiplication = tree.operation(
        OperationType::Multiply,
        tree.number(3),
        tree.number(10)
    );

    ExpressionNode* root = tree.operation(
        OperationType::Divide,
        multiplication,
        tree.number(5)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 6);

    ASSERT_EQ(result.steps.size(), 2);

    EXPECT_EQ(result.steps[0].left, 3);
    EXPECT_EQ(result.steps[0].right, 10);
    EXPECT_EQ(result.steps[0].result, 30);

    EXPECT_EQ(result.steps[1].left, 30);
    EXPECT_EQ(result.steps[1].right, 5);
    EXPECT_EQ(result.steps[1].result, 6);
}

// 24. Выражение с левой и правой частью

TEST(ExpressionSolverTest, SolvesLeftAndRightSubtrees) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* multiplication = tree.operation(
        OperationType::Multiply,
        tree.number(2),
        tree.number(3)
    );

    ExpressionNode* division = tree.operation(
        OperationType::Divide,
        tree.number(8),
        tree.number(4)
    );

    ExpressionNode* root = tree.operation(
        OperationType::Add,
        multiplication,
        division
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 8);

    ASSERT_EQ(result.steps.size(), 3);

    EXPECT_EQ(result.steps[0].result, 6);
    EXPECT_EQ(result.steps[1].result, 2);

    EXPECT_EQ(result.steps[2].left, 6);
    EXPECT_EQ(result.steps[2].right, 2);
    EXPECT_EQ(result.steps[2].result, 8);
}

// 25. Повторный запуск очищает старые шаги

TEST(ExpressionSolverTest, ClearsStepsBeforeNextSolve) {
    TestTree tree;
    ExpressionSolver solver;

    ErrorSet firstErrors;

    ExpressionNode* firstRoot = tree.operation(
        OperationType::Add,
        tree.number(2),
        tree.number(3)
    );

    SolveResult firstResult =
        solver.solve(firstRoot, firstErrors);

    EXPECT_FALSE(firstErrors.hasErrors());
    EXPECT_EQ(firstResult.finalResult, 5);
    ASSERT_EQ(firstResult.steps.size(), 1);

    ErrorSet secondErrors;

    ExpressionNode* secondRoot = tree.number(7);

    SolveResult secondResult =
        solver.solve(secondRoot, secondErrors);

    EXPECT_FALSE(secondErrors.hasErrors());
    EXPECT_EQ(secondResult.finalResult, 7);
    EXPECT_TRUE(secondResult.steps.empty());
}

// Тесты ExpressionSolver::evaluate()
// Функция проверяется через публичный метод solve()

// 1. Пустой узел дерева

TEST(ExpressionEvaluateTest, ReportsErrorForNullNode) {
    ExpressionSolver solver;
    ErrorSet errors;

    SolveResult result = solver.solve(nullptr, errors);

    EXPECT_EQ(result.finalResult, 0);
    EXPECT_TRUE(result.steps.empty());

    EXPECT_TRUE(errors.hasErrors());
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::SyntaxError)
    );
}

// 2. Узел с числом

TEST(ExpressionEvaluateTest, ReturnsNumberNodeValue) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.number(7);

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 7);
    EXPECT_TRUE(result.steps.empty());
}

// 3. Вычисление операции сложения

TEST(ExpressionEvaluateTest, EvaluatesAdditionNode) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::Add,
        tree.number(2),
        tree.number(3)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 5);

    ASSERT_EQ(result.steps.size(), 1);
    EXPECT_EQ(result.steps[0].left, 2);
    EXPECT_EQ(result.steps[0].right, 3);
    EXPECT_EQ(result.steps[0].operation, OperationType::Add);
    EXPECT_EQ(result.steps[0].result, 5);
}

// 4. Вычисление операции вычитания

TEST(ExpressionEvaluateTest, EvaluatesSubtractionNode) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::Subtract,
        tree.number(5),
        tree.number(2)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 3);

    ASSERT_EQ(result.steps.size(), 1);
    EXPECT_EQ(result.steps[0].left, 5);
    EXPECT_EQ(result.steps[0].right, 2);
    EXPECT_EQ(
        result.steps[0].operation,
        OperationType::Subtract
    );
    EXPECT_EQ(result.steps[0].result, 3);
}

// 5. Вычисление операции умножения

TEST(ExpressionEvaluateTest, EvaluatesMultiplicationNode) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::Multiply,
        tree.number(4),
        tree.number(6)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 24);

    ASSERT_EQ(result.steps.size(), 1);
    EXPECT_EQ(result.steps[0].left, 4);
    EXPECT_EQ(result.steps[0].right, 6);
    EXPECT_EQ(
        result.steps[0].operation,
        OperationType::Multiply
    );
    EXPECT_EQ(result.steps[0].result, 24);
}

// 6. Вычисление операции деления

TEST(ExpressionEvaluateTest, EvaluatesDivisionNode) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::Divide,
        tree.number(8),
        tree.number(2)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 4);

    ASSERT_EQ(result.steps.size(), 1);
    EXPECT_EQ(result.steps[0].left, 8);
    EXPECT_EQ(result.steps[0].right, 2);
    EXPECT_EQ(
        result.steps[0].operation,
        OperationType::Divide
    );
    EXPECT_EQ(result.steps[0].result, 4);
}

// 17. Сложное дерево с приоритетом умножения

TEST(ExpressionEvaluateTest, EvaluatesMultiplicationBeforeAddition) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* multiplication = tree.operation(
        OperationType::Multiply,
        tree.number(3),
        tree.number(4)
    );

    ExpressionNode* root = tree.operation(
        OperationType::Add,
        tree.number(2),
        multiplication
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 14);

    ASSERT_EQ(result.steps.size(), 2);

    EXPECT_EQ(result.steps[0].left, 3);
    EXPECT_EQ(result.steps[0].right, 4);
    EXPECT_EQ(result.steps[0].result, 12);

    EXPECT_EQ(result.steps[1].left, 2);
    EXPECT_EQ(result.steps[1].right, 12);
    EXPECT_EQ(result.steps[1].result, 14);
}

// 18. Сложное дерево со скобочным подвыражением

TEST(ExpressionEvaluateTest, EvaluatesBracketSubtreeFirst) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* addition = tree.operation(
        OperationType::Add,
        tree.number(2),
        tree.number(3)
    );

    ExpressionNode* root = tree.operation(
        OperationType::Multiply,
        addition,
        tree.number(4)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 20);

    ASSERT_EQ(result.steps.size(), 2);

    EXPECT_EQ(result.steps[0].result, 5);
    EXPECT_EQ(result.steps[1].left, 5);
    EXPECT_EQ(result.steps[1].right, 4);
    EXPECT_EQ(result.steps[1].result, 20);
}

// 19. Дерево с безопасным вычитанием

TEST(ExpressionEvaluateTest, EvaluatesSafeSubtractionTree) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* addition = tree.operation(
        OperationType::Add,
        tree.number(2),
        tree.number(5)
    );

    ExpressionNode* root = tree.operation(
        OperationType::Subtract,
        addition,
        tree.number(3)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 4);

    ASSERT_EQ(result.steps.size(), 2);

    EXPECT_EQ(result.steps[0].result, 7);
    EXPECT_EQ(result.steps[1].left, 7);
    EXPECT_EQ(result.steps[1].right, 3);
    EXPECT_EQ(result.steps[1].result, 4);
}

// 20. Отрицательный результат в правом поддереве

TEST(ExpressionEvaluateTest, StopsOnNegativeResultInRightSubtree) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* subtraction = tree.operation(
        OperationType::Subtract,
        tree.number(2),
        tree.number(5)
    );

    ExpressionNode* root = tree.operation(
        OperationType::Add,
        tree.number(10),
        subtraction
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_EQ(result.finalResult, 0);

    EXPECT_TRUE(errors.hasErrors());
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::NegativeResult)
    );

    // Ошибочная операция и родительское сложение не сохраняются.
    EXPECT_TRUE(result.steps.empty());
}

// 21. Дробный результат в левом поддереве

TEST(ExpressionEvaluateTest, StopsOnFractionResultInLeftSubtree) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* division = tree.operation(
        OperationType::Divide,
        tree.number(2),
        tree.number(5)
    );

    ExpressionNode* root = tree.operation(
        OperationType::Add,
        division,
        tree.number(3)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_EQ(result.finalResult, 0);

    EXPECT_TRUE(errors.hasErrors());
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::FractionResult)
    );

    EXPECT_TRUE(result.steps.empty());
}

// 22. Деление на ноль в правом поддереве

TEST(ExpressionEvaluateTest, StopsOnDivisionByZeroInRightSubtree) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* division = tree.operation(
        OperationType::Divide,
        tree.number(8),
        tree.number(0)
    );

    ExpressionNode* root = tree.operation(
        OperationType::Add,
        tree.number(3),
        division
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_EQ(result.finalResult, 0);

    EXPECT_TRUE(errors.hasErrors());
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::DivisionByZero)
    );

    EXPECT_TRUE(result.steps.empty());
}

// 23. Дерево с несколькими успешными операциями

TEST(ExpressionEvaluateTest, EvaluatesSeveralSuccessfulOperations) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* multiplication = tree.operation(
        OperationType::Multiply,
        tree.number(2),
        tree.number(3)
    );

    ExpressionNode* division = tree.operation(
        OperationType::Divide,
        tree.number(8),
        tree.number(4)
    );

    ExpressionNode* root = tree.operation(
        OperationType::Add,
        multiplication,
        division
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 8);

    ASSERT_EQ(result.steps.size(), 3);

    EXPECT_EQ(result.steps[0].result, 6);
    EXPECT_EQ(result.steps[1].result, 2);

    EXPECT_EQ(result.steps[2].left, 6);
    EXPECT_EQ(result.steps[2].right, 2);
    EXPECT_EQ(result.steps[2].result, 8);
}

// 24. Ошибка после успешного вычисления левого поддерева

TEST(ExpressionEvaluateTest, KeepsLeftSubtreeStepBeforeRightError) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* multiplication = tree.operation(
        OperationType::Multiply,
        tree.number(2),
        tree.number(3)
    );

    ExpressionNode* division = tree.operation(
        OperationType::Divide,
        tree.number(5),
        tree.number(2)
    );

    ExpressionNode* root = tree.operation(
        OperationType::Add,
        multiplication,
        division
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_EQ(result.finalResult, 0);

    EXPECT_TRUE(errors.hasErrors());
    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::FractionResult)
    );

    // Левое поддерево успело успешно вычислиться.
    ASSERT_EQ(result.steps.size(), 1);

    EXPECT_EQ(result.steps[0].left, 2);
    EXPECT_EQ(result.steps[0].right, 3);
    EXPECT_EQ(result.steps[0].result, 6);
}

// 30. Сложное корректное дерево

TEST(ExpressionEvaluateTest, EvaluatesComplexCorrectTree) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* firstAddition = tree.operation(
        OperationType::Add,
        tree.number(8),
        tree.number(4)
    );

    ExpressionNode* subtraction = tree.operation(
        OperationType::Subtract,
        tree.number(9),
        tree.number(3)
    );

    ExpressionNode* multiplication = tree.operation(
        OperationType::Multiply,
        firstAddition,
        subtraction
    );

    ExpressionNode* division = tree.operation(
        OperationType::Divide,
        multiplication,
        tree.number(6)
    );

    ExpressionNode* secondAddition = tree.operation(
        OperationType::Add,
        tree.number(7),
        tree.number(5)
    );

    ExpressionNode* root = tree.operation(
        OperationType::Add,
        division,
        secondAddition
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 24);

    ASSERT_EQ(result.steps.size(), 6);

    EXPECT_EQ(result.steps[0].result, 12);
    EXPECT_EQ(result.steps[1].result, 6);
    EXPECT_EQ(result.steps[2].result, 72);
    EXPECT_EQ(result.steps[3].result, 12);
    EXPECT_EQ(result.steps[4].result, 12);
    EXPECT_EQ(result.steps[5].result, 24);
}

// Тесты ExpressionSolver::calculate()
// calculate() проверяется через публичный метод solve()

// 1. Сложение

TEST(ExpressionCalculateTest, CalculatesAddition) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::Add,
        tree.number(2),
        tree.number(3)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 5);

    ASSERT_EQ(result.steps.size(), 1);
    EXPECT_EQ(result.steps[0].left, 2);
    EXPECT_EQ(result.steps[0].right, 3);
    EXPECT_EQ(result.steps[0].operation, OperationType::Add);
    EXPECT_EQ(result.steps[0].result, 5);
}

// 2. Вычитание

TEST(ExpressionCalculateTest, CalculatesSubtraction) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::Subtract,
        tree.number(5),
        tree.number(2)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 3);

    ASSERT_EQ(result.steps.size(), 1);
    EXPECT_EQ(result.steps[0].left, 5);
    EXPECT_EQ(result.steps[0].right, 2);
    EXPECT_EQ(
        result.steps[0].operation,
        OperationType::Subtract
    );
    EXPECT_EQ(result.steps[0].result, 3);
}

// 3. Умножение

TEST(ExpressionCalculateTest, CalculatesMultiplication) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::Multiply,
        tree.number(4),
        tree.number(6)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 24);

    ASSERT_EQ(result.steps.size(), 1);
    EXPECT_EQ(result.steps[0].left, 4);
    EXPECT_EQ(result.steps[0].right, 6);
    EXPECT_EQ(
        result.steps[0].operation,
        OperationType::Multiply
    );
    EXPECT_EQ(result.steps[0].result, 24);
}

// 4. Деление

TEST(ExpressionCalculateTest, CalculatesDivision) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::Divide,
        tree.number(8),
        tree.number(2)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 4);

    ASSERT_EQ(result.steps.size(), 1);
    EXPECT_EQ(result.steps[0].left, 8);
    EXPECT_EQ(result.steps[0].right, 2);
    EXPECT_EQ(
        result.steps[0].operation,
        OperationType::Divide
    );
    EXPECT_EQ(result.steps[0].result, 4);
}

// 5. Вычитание с нулевым результатом

TEST(ExpressionCalculateTest, AllowsZeroSubtractionResult) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::Subtract,
        tree.number(5),
        tree.number(5)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 0);

    ASSERT_EQ(result.steps.size(), 1);
    EXPECT_EQ(result.steps[0].result, 0);
}

// 6. Вычитание с отрицательным результатом

TEST(ExpressionCalculateTest, ReportsNegativeResult) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::Subtract,
        tree.number(2),
        tree.number(5)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_EQ(result.finalResult, 0);
    EXPECT_TRUE(result.steps.empty());

    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::NegativeResult)
    );
}

// 7. Деление с дробным результатом

TEST(ExpressionCalculateTest, ReportsFractionResult) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::Divide,
        tree.number(2),
        tree.number(5)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_EQ(result.finalResult, 0);
    EXPECT_TRUE(result.steps.empty());

    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::FractionResult)
    );
}

// 8. Деление на ноль

TEST(ExpressionCalculateTest, ReportsDivisionByZero) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::Divide,
        tree.number(8),
        tree.number(0)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_EQ(result.finalResult, 0);
    EXPECT_TRUE(result.steps.empty());

    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::DivisionByZero)
    );
}

// 9. Превышение допустимого значения при сложении

TEST(ExpressionCalculateTest, ReportsAdditionOverflow) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::Add,
        tree.number(1000000000),
        tree.number(1)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_EQ(result.finalResult, 0);
    EXPECT_TRUE(result.steps.empty());

    EXPECT_TRUE(
        HasErrorType(
            errors,
            ErrorType::IntermediateOverflow
        )
    );
}

// 10. Превышение допустимого значения при умножении

TEST(ExpressionCalculateTest, ReportsMultiplicationOverflow) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::Multiply,
        tree.number(500000),
        tree.number(500000)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_EQ(result.finalResult, 0);
    EXPECT_TRUE(result.steps.empty());

    EXPECT_TRUE(
        HasErrorType(
            errors,
            ErrorType::IntermediateOverflow
        )
    );
}

// 11. Неизвестный тип операции

TEST(ExpressionCalculateTest, ReportsUnknownOperation) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::None,
        tree.number(2),
        tree.number(3)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_EQ(result.finalResult, 0);
    EXPECT_TRUE(result.steps.empty());

    EXPECT_TRUE(
        HasErrorType(errors, ErrorType::UnknownOperation)
    );
}

// 12. Деление нуля на число

TEST(ExpressionCalculateTest, DividesZeroByNumber) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::Divide,
        tree.number(0),
        tree.number(5)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 0);

    ASSERT_EQ(result.steps.size(), 1);
    EXPECT_EQ(result.steps[0].left, 0);
    EXPECT_EQ(result.steps[0].right, 5);
    EXPECT_EQ(result.steps[0].result, 0);
}

// 13. Умножение на единицу

TEST(ExpressionCalculateTest, MultipliesByOne) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::Multiply,
        tree.number(25),
        tree.number(1)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 25);

    ASSERT_EQ(result.steps.size(), 1);
    EXPECT_EQ(result.steps[0].result, 25);
}

// 14. Деление числа на единицу

TEST(ExpressionCalculateTest, DividesByOne) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::Divide,
        tree.number(25),
        tree.number(1)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 25);

    ASSERT_EQ(result.steps.size(), 1);
    EXPECT_EQ(result.steps[0].result, 25);
}

// 15. Граничное допустимое значение результата

TEST(ExpressionCalculateTest, AcceptsMaximumIntermediateResult) {
    TestTree tree;
    ExpressionSolver solver;
    ErrorSet errors;

    ExpressionNode* root = tree.operation(
        OperationType::Add,
        tree.number(999999999),
        tree.number(1)
    );

    SolveResult result = solver.solve(root, errors);

    EXPECT_FALSE(errors.hasErrors());
    EXPECT_EQ(result.finalResult, 1000000000);

    ASSERT_EQ(result.steps.size(), 1);
    EXPECT_EQ(result.steps[0].result, 1000000000);
}