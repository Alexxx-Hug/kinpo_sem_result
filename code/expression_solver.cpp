#include "expression_solver.h"

#include <string>

long long ExpressionSolver::calculate(
    long long left,
    long long right,
    OperationType operation,
    ErrorSet& errors
) const {
    // создаём переменную для результата текущей операции
    long long result = 0;

    // выполняем сложение двух операндов
    if (operation == OperationType::Add) {
        result = left + right;
    } else if (operation == OperationType::Subtract) {
        // проверяем, не приведёт ли вычитание к отрицательному результату
        if (left < right) {
            errors.add(
                ErrorType::NegativeResult,
                "Операция " + std::to_string(left) + " - " + std::to_string(right) +
                " приводит к отрицательному результату. По условию задачи отрицательные числа недопустимы."
            );

            return 0;
        }

        // выполняем вычитание после проверки
        result = left - right;
    } else if (operation == OperationType::Multiply) {
        // заранее проверяем переполнение при умножении
        if (left != 0 && right > MaxIntermediateValue / left) {
            errors.add(
                ErrorType::IntermediateOverflow,
                "Обнаружена операция, которая в результате даст число, превышающее допустимое значение."
            );

            return 0;
        }

        // выполняем умножение после проверки
        result = left * right;
    } else if (operation == OperationType::Divide) {
        // проверяем деление на ноль
        if (right == 0) {
            errors.add(
                ErrorType::DivisionByZero,
                "Обнаружено деление на ноль. Выполнение выражения невозможно."
            );

            return 0;
        }

        // проверяем, что деление не даст дробный результат
        if (left % right != 0) {
            errors.add(
                ErrorType::FractionResult,
                "Операция " + std::to_string(left) + " / " + std::to_string(right) +
                " приводит к дробному результату. По условию задачи дробные числа недопустимы."
            );

            return 0;
        }

        // выполняем деление после проверок
        result = left / right;
    } else {
        // фиксируем ошибку, если операция не относится к поддерживаемым типам
        errors.add(
            ErrorType::UnknownOperation,
            "Во входном выражении обнаружена неизвестная операция."
        );

        return 0;
    }

    // проверяем итоговый результат операции на превышение допустимого значения
    if (result > MaxIntermediateValue) {
        errors.add(
            ErrorType::IntermediateOverflow,
            "Обнаружена операция, которая в результате даст число, превышающее допустимое значение."
        );

        return 0;
    }

    // возвращаем результат успешно выполненной операции
    return result;
}

void ExpressionSolver::addStep(
    long long left,
    long long right,
    OperationType operation,
    long long result
) {
    // сохраняем выполненное арифметическое действие в список шагов
    steps.emplace_back(left, right, operation, result);
}

long long ExpressionSolver::evaluate(
    const ExpressionNode* node,
    ErrorSet& errors
) {
    // проверяем, что текущий узел дерева существует
    if (node == nullptr) {
        errors.add(
            ErrorType::SyntaxError,
            "Во входном выражении обнаружена синтаксическая ошибка."
        );

        return 0;
    }

    // если узел является числом, сразу возвращаем его значение
    if (node->type == NodeType::Number) {
        return node->value;
    }

    // рекурсивно вычисляем левое поддерево
    long long leftValue = evaluate(node->left, errors);

    // если при вычислении левого поддерева возникла ошибка, останавливаем вычисление
    if (errors.hasErrors()) {
        return 0;
    }

    // рекурсивно вычисляем правое поддерево
    long long rightValue = evaluate(node->right, errors);

    // если при вычислении правого поддерева возникла ошибка, останавливаем вычисление
    if (errors.hasErrors()) {
        return 0;
    }

    // выполняем операцию текущего узла над результатами поддеревьев
    long long result = calculate(
        leftValue,
        rightValue,
        node->operation,
        errors
    );

    // если операция завершилась ошибкой, не сохраняем её как успешный шаг
    if (errors.hasErrors()) {
        return 0;
    }

    // сохраняем успешно выполненный шаг вычисления
    addStep(leftValue, rightValue, node->operation, result);

    // возвращаем результат текущего поддерева
    return result;
}

SolveResult ExpressionSolver::solve(
    const ExpressionNode* root,
    ErrorSet& errors
) {
    // очищаем старые шаги перед новым вычислением
    steps.clear();

    // создаём структуру для итогового результата
    SolveResult result;

    // проверяем, что корень дерева существует
    if (root == nullptr) {
        errors.add(
            ErrorType::SyntaxError,
            "Во входном выражении обнаружена синтаксическая ошибка."
        );

        result.finalResult = 0;
        result.steps = steps;
        return result;
    }

    // запускаем рекурсивное вычисление дерева
    result.finalResult = evaluate(root, errors);

    // сохраняем накопленные шаги в результат
    result.steps = steps;

    // возвращаем итог вычисления выражения
    return result;
}