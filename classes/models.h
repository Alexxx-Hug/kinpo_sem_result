#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <vector>

enum class TokenType {
    Number,
    Operation,
    LeftBracket,
    RightBracket,
    End
};

enum class OperationType {
    Add,
    Subtract,
    Multiply,
    Divide,
    None
};

struct Token {
    TokenType type = TokenType::End;
    std::string text;
    long long value = 0;
    OperationType operation = OperationType::None;

    Token() = default;

    Token(TokenType tokenType, const std::string& tokenText)
        : type(tokenType),
          text(tokenText),
          value(0),
          operation(OperationType::None) {}

    Token(TokenType tokenType, const std::string& tokenText, long long tokenValue)
        : type(tokenType),
          text(tokenText),
          value(tokenValue),
          operation(OperationType::None) {}

    Token(TokenType tokenType, const std::string& tokenText, OperationType operationValue)
        : type(tokenType),
          text(tokenText),
          value(0),
          operation(operationValue) {}
};

struct CalculationStep {
    long long left = 0;
    long long right = 0;
    OperationType operation = OperationType::None;
    long long result = 0;

    CalculationStep() = default;

    CalculationStep(
        long long leftValue,
        long long rightValue,
        OperationType operationValue,
        long long resultValue
    )
        : left(leftValue),
          right(rightValue),
          operation(operationValue),
          result(resultValue) {}
};

struct SolveResult {
    long long finalResult = 0;
    std::vector<CalculationStep> steps;
};

#endif