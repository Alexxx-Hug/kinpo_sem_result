#include "tokenizer.h"

#include <cctype>
#include <string>
#include <vector>

OperationType Tokenizer::operationFromChar(char symbol) const {
    // проверяем символ сложения
    if (symbol == '+') {
        return OperationType::Add;
    }

    // проверяем символ вычитания
    if (symbol == '-') {
        return OperationType::Subtract;
    }

    // проверяем символ умножения
    if (symbol == '*') {
        return OperationType::Multiply;
    }

    // проверяем символ деления
    if (symbol == '/') {
        return OperationType::Divide;
    }

    // возвращаем None, если символ не является поддерживаемой операцией
    return OperationType::None;
}

std::vector<Token> Tokenizer::tokenize(const std::string& expression,ErrorSet& errors) const {
    // создаём список токенов, который будет заполнен при разборе строки
    std::vector<Token> tokens;

    // проверяем полностью пустую строку
    if (expression.empty()) {
        errors.add(
            ErrorType::EmptyInput,
            "Входная строка пуста. Необходимо ввести математическое выражение."
        );

        // добавляем служебный токен конца, чтобы результат всегда имел корректное завершение
        tokens.emplace_back(TokenType::End, "");
        return tokens;
    }

    // проверяем превышение максимальной длины выражения
    if (expression.size() > MaxExpressionLength) {
        errors.add(
            ErrorType::ExpressionTooLong,
            "Длина входного выражения превышает допустимый предел. Сократите выражение."
        );
    }

    // флаг показывает, есть ли в строке хотя бы один символ кроме пробела
    bool hasNonSpace = false;

    // ищем первый непробельный символ во входной строке
    for (char symbol : expression) {
        if (!std::isspace(static_cast<unsigned char>(symbol))) {
            hasNonSpace = true;
            break;
        }
    }

    // если строка состоит только из пробелов, фиксируем ошибку
    if (!hasNonSpace) {
        errors.add(
            ErrorType::OnlySpacesInput,
            "Входная строка не содержит выражения. Удалите лишние пробелы и введите данные."
        );

        // добавляем служебный токен конца для корректного завершения списка
        tokens.emplace_back(TokenType::End, "");
        return tokens;
    }

    // проходим по строке вручную, чтобы собирать числа из нескольких цифр
    for (size_t i = 0; i < expression.size();) {
        // берём текущий символ выражения
        char current = expression[i];
        // пропускаем пробельные символы
        if (std::isspace(static_cast<unsigned char>(current))) {
            ++i;
            continue;
        }

        // точка или запятая означают попытку записать дробное число
        if (current == '.' || current == ',') {
            errors.add(
                ErrorType::NotIntegerNumber,
                "Обнаружено нецелое число. Программа принимает только целые положительные числа."
            );
            ++i;
            continue;
        }

        // проверяем, является ли текущий символ цифрой
        bool isDigit = std::isdigit(static_cast<unsigned char>(current));

        // проверяем, является ли текущий символ арифметической операцией
        bool isOperation =
            current == '+' ||
            current == '-' ||
            current == '*' ||
            current == '/';

        // проверяем, является ли текущий символ скобкой
        bool isBracket =
            current == '(' ||
            current == ')';

        // если символ не относится ни к одной допустимой категории, добавляем ошибку
        if (!isDigit && !isOperation && !isBracket) {
            std::string message = "Во входном выражении обнаружен недопустимый символ «";
            message += current;
            message += "». Разрешены только цифры, пробелы и знаки операций +, -, *, /.";
            errors.add(ErrorType::InvalidSymbol, message);
            ++i;
            continue;
        }

        // если текущий символ является цифрой, начинаем собирать целое число
        if (isDigit) {
            std::string numberText;
            // считываем подряд идущие цифры в одну строку числа
            while (i < expression.size() && std::isdigit(static_cast<unsigned char>(expression[i]))) {
                numberText += expression[i];
                ++i;
            }

            // переменная для числового значения токена
            long long value = 0;

            // преобразуем текст числа в long long
            try {
                value = std::stoll(numberText);
            } catch (...) {
                errors.add(
                    ErrorType::NumberOutOfRange,
                    "Обнаружено число, выходящее за допустимый диапазон. Введите значение меньшего размера."
                );

                continue;
            }

            // ноль запрещён по условию, так как операнды должны быть положительными
            if (value == 0) {
                errors.add(
                    ErrorType::ZeroOperand,
                    "Операнд «0» не является положительным целым числом. Введите число больше нуля."
                );
            }

            // проверяем верхнюю границу допустимого значения числа
            if (value > MaxInputNumber) {
                errors.add(
                    ErrorType::NumberOutOfRange,
                    "Обнаружено число, выходящее за допустимый диапазон. Введите значение меньшего размера."
                );
            }

            // добавляем токен числа в итоговый список
            tokens.emplace_back(TokenType::Number, numberText, value);
            continue;
        }

        // если текущий символ является операцией, создаём токен операции
        if (isOperation) {
            tokens.emplace_back(
                TokenType::Operation,
                std::string(1, current),
                operationFromChar(current)
            );
            ++i;
            continue;
        }

        // если встретили открывающую скобку, добавляем соответствующий токен
        if (current == '(') {
            tokens.emplace_back(TokenType::LeftBracket, "(");
            ++i;
            continue;
        }

        // если встретили закрывающую скобку, добавляем соответствующий токен
        if (current == ')') {
            tokens.emplace_back(TokenType::RightBracket, ")");
            ++i;
            continue;
        }
    }

    // добавляем служебный токен конца выражения
    tokens.emplace_back(TokenType::End, "");

    // возвращаем сформированный список токенов
    return tokens;
}