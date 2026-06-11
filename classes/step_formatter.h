#ifndef STEP_FORMATTER_H
#define STEP_FORMATTER_H

#include "app_error.h"
#include "models.h"

#include <string>
#include <vector>

/**
 * @brief Класс для форматирования шагов и ошибок
 * 
 * Преобразует вводные шаги расчёта и ошибки в читаемые строки.
 */
class StepFormatter {
public:
    /**
     * @brief Форматирует последовательность шагов вычисления в текстовом формате
     * @param steps Последовательность шагов расчёта
     * @param finalResult Финальный результат вычисления
     * @return Вектор сформатированных строк
     */
    std::vector<std::string> formatSteps(
        const std::vector<CalculationStep>& steps,
        long long finalResult
    ) const;

    /**
     * @brief Форматирует набор ошибок в текстовые строки
     * @param errors Набор ошибок
     * @return Вектор строк с описанием ошибок
     */
    std::vector<std::string> formatErrors(
        const ErrorSet& errors
    ) const;

private:
    /**
     * @brief Преобразует арифметическую операцию в символ
     * @param operation Тип операции
     * @return Символ операции (+, -, *, /)
     */
    char operationToChar(OperationType operation) const;
};

#endif