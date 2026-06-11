#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <vector>

/**
 * @brief Тип токена - элемента выражения
 */
enum class TokenType {
    Number,           // Числовой токен
    Operation,        // Операция (+, -, *, /)
    LeftBracket,      // Открывающая скобка
    RightBracket,     // Закрывающая скобка
    End               // Конец выражения
};

/**
 * @brief Тип арифметической операции
 */
enum class OperationType {
    Add,      // Операция сложения
    Subtract, // Операция вычитания
    Multiply, // Операция умножения
    Divide,   // Операция деления
    None      // Отсутствие операции
};

/**
 * @brief Структура для представления токена
 * 
 * Содержит информацию о типе токена, его текстовом представлении,
 * числовом значении (для чисел) или типе операции (для операций).
 */
struct Token {
    TokenType type = TokenType::End;           // Тип токена
    std::string text;                          // Текстовое представление токена
    long long value = 0;                       // Числовое значение (для токенов Number)
    OperationType operation = OperationType::None; // Тип операции (для токенов Operation)

    Token() = default;

    /**
     * @brief Конструктор токена с типом и текстом
     * @param tokenType Тип токена
     * @param tokenText Текстовое представление
     */
    Token(TokenType tokenType, const std::string& tokenText)
        : type(tokenType),
          text(tokenText),
          value(0),
          operation(OperationType::None) {}

    /**
     * @brief Конструктор токена-числа
     * @param tokenType Тип токена (обычно Number)
     * @param tokenText Текстовое представление числа
     * @param tokenValue Числовое значение
     */
    Token(TokenType tokenType, const std::string& tokenText, long long tokenValue)
        : type(tokenType),
          text(tokenText),
          value(tokenValue),
          operation(OperationType::None) {}

    /**
     * @brief Конструктор токена-операции
     * @param tokenType Тип токена (обычно Operation)
     * @param tokenText Символ операции
     * @param operationValue Тип арифметической операции
     */
    Token(TokenType tokenType, const std::string& tokenText, OperationType operationValue)
        : type(tokenType),
          text(tokenText),
          value(0),
          operation(operationValue) {}
};

/**
 * @brief Структура для представления одного шага расчёта
 * 
 * Хранит два операнда, тип операции и результат вычисления.
 */
struct CalculationStep {
    long long left = 0;                        // Левый операнд
    long long right = 0;                       // Правый операнд
    OperationType operation = OperationType::None; // Тип операции
    long long result = 0;                      // Результат вычисления

    CalculationStep() = default;

    /**
     * @brief Конструктор шага расчёта
     * @param leftValue Левый операнд
     * @param rightValue Правый операнд
     * @param operationValue Тип операции
     * @param resultValue Результат вычисления
     */
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

/**
 * @brief Структура для представления результата решения выражения
 * 
 * Содержит финальный результат и последовательность шагов расчёта.
 */
struct SolveResult {
    long long finalResult = 0;                 // Финальный результат вычисления
    std::vector<CalculationStep> steps;       // Последовательность шагов расчёта
};

#endif