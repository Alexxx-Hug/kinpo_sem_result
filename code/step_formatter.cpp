#include "step_formatter.h"

#include <sstream>
#include <string>
#include <vector>

char StepFormatter::operationToChar(OperationType operation) const {
    if (operation == OperationType::Add) {
        return '+';
    }

    if (operation == OperationType::Subtract) {
        return '-';
    }

    if (operation == OperationType::Multiply) {
        return '*';
    }

    if (operation == OperationType::Divide) {
        return '/';
    }

    return '?';
}

std::vector<std::string> StepFormatter::formatSteps(const std::vector<CalculationStep>& steps,long long finalResult) const {
    std::vector<std::string> lines;

    if (steps.empty()) {
        lines.push_back("1) " + std::to_string(finalResult));
        return lines;
    }

    for (size_t i = 0; i < steps.size(); ++i) {
        const CalculationStep& step = steps[i];

        std::ostringstream output;

        output << i + 1 << ") "
               << step.left
               << " "
               << operationToChar(step.operation)
               << " "
               << step.right
               << " = "
               << step.result;

        lines.push_back(output.str());
    }

    return lines;
}

std::vector<std::string> StepFormatter::formatErrors(const ErrorSet& errors) const {
    std::vector<std::string> lines;

    for (const AppError& error : errors.getErrors()) {
        lines.push_back("Ошибка: " + error.getMessage());
    }

    return lines;
}