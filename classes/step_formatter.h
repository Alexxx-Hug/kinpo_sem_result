#ifndef STEP_FORMATTER_H
#define STEP_FORMATTER_H

#include "app_error.h"
#include "models.h"

#include <string>
#include <vector>

class StepFormatter {
public:
    std::vector<std::string> formatSteps(
        const std::vector<CalculationStep>& steps,
        long long finalResult
    ) const;

    std::vector<std::string> formatErrors(
        const ErrorSet& errors
    ) const;

private:
    char operationToChar(OperationType operation) const;
};

#endif