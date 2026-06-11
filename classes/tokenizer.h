#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "app_error.h"
#include "models.h"

#include <string>
#include <vector>

/**
 * @brief Класс для токенизации выражений
 * 
 * Преобразует строковые выражения в последовательность токенов.
 */
class Tokenizer {
public:
    /**
     * @brief Преобразует выражение в последовательность токенов
     * @param expression Выражение в виде строки
     * @param errors Набор ошибок для сбора ошибок
     * @return Вектор найденных токенов
     */
    std::vector<Token> tokenize(
        const std::string& expression,
        ErrorSet& errors
    ) const;

private:
    static constexpr int MaxExpressionLength = 100;     // Максимальная длина выражения
    static constexpr long long MaxInputNumber = 500000; // Максимальное входное число

    /**
     * @brief Преобразует символ в тип операции
     * @param symbol Символ операции
     * @return Найденный тип операции
     */
    OperationType operationFromChar(char symbol) const;
};

#endif