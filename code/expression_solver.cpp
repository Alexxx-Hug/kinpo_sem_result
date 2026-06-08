#include "expression_solver.h"

#include <string>

long long ExpressionSolver::calculate(
    long long left,
    long long right,
    OperationType operation,
    ErrorSet& errors
) const {
    long long result = 0;

    if (operation == OperationType::Add) {
        result = left + right;
    } else if (operation == OperationType::Subtract) {
        if (left < right) {
            errors.add(
                ErrorType::NegativeResult,
                "Операция " + std::to_string(left) + " - " + std::to_string(right) +
                " приводит к отрицательному результату. По условию задачи отрицательные числа недопустимы."
            );

            return 0;
        }

        result = left - right;
    } else if (operation == OperationType::Multiply) {
        if (left != 0 && right > MaxIntermediateValue / left) {
            errors.add(
                ErrorType::IntermediateOverflow,
                "Обнаружена операция, которая в результате даст число, превышающее допустимое значение."
            );

            return 0;
        }

        result = left * right;
    } else if (operation == OperationType::Divide) {
        if (right == 0) {
            errors.add(
                ErrorType::DivisionByZero,
                "Обнаружено деление на ноль. Выполнение выражения невозможно."
            );

            return 0;
        }

        if (left % right != 0) {
            errors.add(
                ErrorType::FractionResult,
                "Операция " + std::to_string(left) + " / " + std::to_string(right) +
                " приводит к дробному результату. По условию задачи дробные числа недопустимы."
            );

            return 0;
        }

        result = left / right;
    } else {
        errors.add(
            ErrorType::SyntaxError,
            "Во входном выражении обнаружена неизвестная операция."
        );

        return 0;
    }

    if (result > MaxIntermediateValue) {
        errors.add(
            ErrorType::IntermediateOverflow,
            "Обнаружена операция, которая в результате даст число, превышающее допустимое значение."
        );

        return 0;
    }

    return result;
}