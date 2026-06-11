#ifndef EXPRESSION_SOLVER_H
#define EXPRESSION_SOLVER_H

#include "app_error.h"
#include "expression_node.h"
#include "models.h"

#include <vector>

/**
 * @brief Класс для вычисления выражения
 * 
 * Выполняет арифметические операции на абстрактном синтаксическом дереве
 * и отслеживает все шаги вычисления.
 */
class ExpressionSolver {
public:
    /**
     * @brief Вычислить выражение со сбережением этапов
     * @param root Корень абстрактного синтаксического дерева
     * @param errors Набор ошибок
     * @return Объект с результатом и историей шагов
     */
    SolveResult solve(
        const ExpressionNode* root,
        ErrorSet& errors
    );

private:
    static constexpr long long MaxIntermediateValue = 1000000000LL; // Максимальное промежуточное значение

    std::vector<CalculationStep> steps; // Последовательность шагов вычисления

    /**
     * @brief Основной метод для вычисления значения узла
     * @param node Узел дерева
     * @param errors Набор ошибок
     * @return Значение узла
     */
    long long evaluate(
        const ExpressionNode* node,
        ErrorSet& errors
    );

    /**
     * @brief Выполнить двоичную арифметическую операцию
     * @param left Левый операнд
     * @param right Правый операнд
     * @param operation Тип арифметической операции
     * @param errors Набор ошибок
     * @return Результат операции
     */
    long long calculate(
        long long left,
        long long right,
        OperationType operation,
        ErrorSet& errors
    ) const;

    /**
     * @brief Добавить новый шаг в список для отслеживания
     * @param left Левый операнд
     * @param right Правый операнд
     * @param operation Выполненная операция
     * @param result Результат операции
     */
    void addStep(
        long long left,
        long long right,
        OperationType operation,
        long long result
    );
};

#endif