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