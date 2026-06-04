#include "tokenizer.h"

#include <cctype>

#include <string>

#include <vector>

OperationType Tokenizer::operationFromChar(char symbol) const {

    if (symbol == '+') {

        return OperationType::Add;

    }

    if (symbol == '-') {

        return OperationType::Subtract;

    }

    if (symbol == '*') {

        return OperationType::Multiply;

    }

    if (symbol == '/') {

        return OperationType::Divide;

    }

    return OperationType::None;

}

std::vector<Token> Tokenizer::tokenize(

    const std::string& expression,

    ErrorSet& errors

) const {

    std::vector<Token> tokens;

    if (expression.empty()) {

        errors.add(

            ErrorType::EmptyInput,

            "Входная строка пуста. Необходимо ввести математическое выражение."

        );

        tokens.emplace_back(TokenType::End, "");

        return tokens;

    }

    if (expression.size() > MaxExpressionLength) {

        errors.add(

            ErrorType::ExpressionTooLong,

            "Длина входного выражения превышает допустимый предел. Сократите выражение."

        );

    }

    bool hasNonSpace = false;

    for (char symbol : expression) {

        if (!std::isspace(static_cast<unsigned char>(symbol))) {

            hasNonSpace = true;

            break;

        }

    }

    if (!hasNonSpace) {

        errors.add(

            ErrorType::OnlySpacesInput,

            "Входная строка не содержит выражения. Удалите лишние пробелы и введите данные."

        );

        tokens.emplace_back(TokenType::End, "");

        return tokens;

    }

    for (size_t i = 0; i < expression.size();) {

        char current = expression[i];

        if (std::isspace(static_cast<unsigned char>(current))) {

            ++i;

            continue;

        }

        if (current == '.' || current == ',') {

            errors.add(

                ErrorType::NotIntegerNumber,

                "Обнаружено нецелое число. Программа принимает только целые положительные числа."

            );

            ++i;

            continue;

        }

        bool isDigit = std::isdigit(static_cast<unsigned char>(current));

        bool isOperation =

            current == '+' ||

            current == '-' ||

            current == '*' ||

            current == '/';

        bool isBracket =

            current == '(' ||

            current == ')';

        if (!isDigit && !isOperation && !isBracket) {

            std::string message = "Во входном выражении обнаружен недопустимый символ «";

            message += current;

            message += "». Разрешены только цифры, пробелы и знаки операций +, -, *, /.";

            errors.add(ErrorType::InvalidSymbol, message);

            ++i;

            continue;

        }

        if (isDigit) {

            std::string numberText;

            while (

                i < expression.size() &&

                std::isdigit(static_cast<unsigned char>(expression[i]))

            ) {

                numberText += expression[i];

                ++i;

            }

            long long value = 0;

            try {

                value = std::stoll(numberText);

            } catch (...) {

                errors.add(

                    ErrorType::NumberOutOfRange,

                    "Обнаружено число, выходящее за допустимый диапазон. Введите значение меньшего размера."

                );

                continue;

            }

            if (value == 0) {

                errors.add(

                    ErrorType::ZeroOperand,

                    "Операнд «0» не является положительным целым числом. Введите число больше нуля."

                );

            }

            if (value > MaxInputNumber) {

                errors.add(

                    ErrorType::NumberOutOfRange,

                    "Обнаружено число, выходящее за допустимый диапазон. Введите значение меньшего размера."

                );

            }

            tokens.emplace_back(TokenType::Number, numberText, value);

            continue;

        }

        if (isOperation) {

            tokens.emplace_back(

                TokenType::Operation,

                std::string(1, current),

                operationFromChar(current)

            );

            ++i;

            continue;

        }

        if (current == '(') {

            tokens.emplace_back(TokenType::LeftBracket, "(");

            ++i;

            continue;

        }

        if (current == ')') {

            tokens.emplace_back(TokenType::RightBracket, ")");

            ++i;

            continue;

        }

    }

    tokens.emplace_back(TokenType::End, "");

    return tokens;

}