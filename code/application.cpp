#include "application.h"

#include "app_error.h"
#include "expression_node.h"
#include "expression_solver.h"
#include "file_service.h"
#include "parser.h"
#include "step_formatter.h"
#include "tokenizer.h"

#include <iostream>
#include <string>
#include <vector>

int Application::run(int argc, char* argv[]) {
    FileService fileService;
    StepFormatter formatter;
    ErrorSet errors;

    if (argc != 3) {
        errors.add(
            ErrorType::InvalidArgumentCount,
            "Кол-во принимаемых аргументов должно быть равно 2"
        );

        for (const std::string& line : formatter.formatErrors(errors)) {
            std::cerr << line << std::endl;
        }

        return 1;
    }

    const std::string inputPath = argv[1];
    const std::string outputPath = argv[2];

    std::vector<std::string> outputLines;

    Tokenizer tokenizer;
    Parser parser;
    ExpressionSolver solver;

    std::string expression = fileService.readAll(inputPath, errors);

    while (
        !expression.empty() &&
        (expression.back() == '\n' || expression.back() == '\r')
    ) {
        expression.pop_back();
    }

    outputLines.push_back("Выражение: " + expression);

    if (!errors.hasErrors()) {
        std::vector<Token> tokens = tokenizer.tokenize(expression, errors);

        if (!errors.hasErrors()) {
            ExpressionNode* root = parser.parse(tokens, errors);

            if (!errors.hasErrors()) {
                SolveResult solveResult = solver.solve(root, errors);

                std::vector<std::string> resultLines = formatter.formatSteps(
                    solveResult.steps,
                    solveResult.finalResult
                );

                outputLines.insert(
                    outputLines.end(),
                    resultLines.begin(),
                    resultLines.end()
                );
            }
        }
    }

    if (errors.hasErrors()) {
        std::vector<std::string> errorLines = formatter.formatErrors(errors);

        outputLines.insert(
            outputLines.end(),
            errorLines.begin(),
            errorLines.end()
        );
    }

    ErrorSet writeErrors;
    fileService.writeLines(outputPath, outputLines, writeErrors);

    if (writeErrors.hasErrors()) {
        for (const std::string& line : formatter.formatErrors(writeErrors)) {
            std::cerr << line << std::endl;
        }

        return 1;
    }

    return errors.hasErrors() ? 1 : 0;
}