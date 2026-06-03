#ifndef APP_ERROR_H
#define APP_ERROR_H

#include <string>
#include <vector>

enum class ErrorType {
    FileOpenError,
    FileWriteError,

    EmptyInput,
    OnlySpacesInput,
    InvalidSymbol,
    NotIntegerNumber,
    NegativeInputNumber,
    ZeroOperand,
    NumberOutOfRange,
    ExpressionTooLong,

    SyntaxError,
    BracketBalanceError,
    EmptyBrackets,
    MissingOperation,
    MissingOperand,

    DivisionByZero,
    NegativeResult,
    FractionResult,
    IntermediateOverflow,

    InvalidArgumentCount,
    UnknownError
};

class AppError {
public:
    AppError(ErrorType errorType, const std::string& errorMessage)
        : type(errorType), message(errorMessage) {}

    ErrorType getType() const {
        return type;
    }

    std::string getMessage() const {
        return message;
    }

private:
    ErrorType type;
    std::string message;
};

class ErrorSet {
public:
    void add(ErrorType type, const std::string& message) {
        errors.emplace_back(type, message);
    }

    bool hasErrors() const {
        return !errors.empty();
    }

    const std::vector<AppError>& getErrors() const {
        return errors;
    }

    void clear() {
        errors.clear();
    }

private:
    std::vector<AppError> errors;
};

#endif