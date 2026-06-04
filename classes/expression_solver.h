#ifndef EXPRESSION_SOLVER_H
#define EXPRESSION_SOLVER_H

#include "app_error.h"
#include "expression_node.h"
#include "models.h"

#include <vector>

class ExpressionSolver {
public:
    SolveResult solve(
        const ExpressionNode* root,
        ErrorSet& errors
    );

private:
    static constexpr long long MaxIntermediateValue = 1000000000LL;

    std::vector<CalculationStep> steps;

    long long evaluate(
        const ExpressionNode* node,
        ErrorSet& errors
    );

    long long calculate(
        long long left,
        long long right,
        OperationType operation,
        ErrorSet& errors
    ) const;

    void addStep(
        long long left,
        long long right,
        OperationType operation,
        long long result
    );
};

#endif