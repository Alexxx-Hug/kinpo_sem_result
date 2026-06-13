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
    // создаём сервис для чтения и записи файлов
    FileService fileService;

    // создаём форматтер для преобразования шагов и ошибок в строки
    StepFormatter formatter;

    // создаём общий набор ошибок для выполнения программы
    ErrorSet errors;

    // проверяем количество аргументов командной строки
    if (argc != 3) {
        errors.add(
            ErrorType::InvalidArgumentCount,
            "Кол-во принимаемых аргументов должно быть равно 2"
        );

        // выводим ошибку в консоль, так как путь к выходному файлу может быть не передан
        for (const std::string& line : formatter.formatErrors(errors)) {
            std::cerr << line << std::endl;
        }

        return 1;
    }

    // сохраняем путь к входному файлу
    const std::string inputPath = argv[1];

    // сохраняем путь к выходному файлу
    const std::string outputPath = argv[2];

    // создаём список строк, которые затем будут записаны в выходной файл
    std::vector<std::string> outputLines;

    // создаём объект для лексического анализа выражения
    Tokenizer tokenizer;

    // создаём объект для построения дерева выражения
    Parser parser;

    // создаём объект для вычисления дерева выражения
    ExpressionSolver solver;

    // считываем выражение из входного файла
    std::string expression = fileService.readAll(inputPath, errors);

    // удаляем переносы строки в конце выражения после чтения файла
    while (
        !expression.empty() &&
        (expression.back() == '\n' || expression.back() == '\r')
    ) {
        expression.pop_back();
    }

    // добавляем исходное выражение в выходной файл
    outputLines.push_back("Выражение: " + expression);

    // продолжаем работу только если чтение файла прошло без ошибок
    if (!errors.hasErrors()) {
        // разбиваем входное выражение на токены
        std::vector<Token> tokens = tokenizer.tokenize(expression, errors);

        // продолжаем работу только если лексический анализ прошёл без ошибок
        if (!errors.hasErrors()) {
            // строим дерево арифметического выражения
            ExpressionNode* root = parser.parse(tokens, errors);

            // продолжаем работу только если дерево построено без ошибок
            if (!errors.hasErrors()) {
                // вычисляем выражение и получаем итоговый результат со списком шагов
                SolveResult solveResult = solver.solve(root, errors);

                // преобразуем шаги вычисления в строки для вывода
                std::vector<std::string> resultLines = formatter.formatSteps(
                    solveResult.steps,
                    solveResult.finalResult
                );

                // добавляем строки результата в общий список вывода
                outputLines.insert(
                    outputLines.end(),
                    resultLines.begin(),
                    resultLines.end()
                );
            }
        }
    }

    // если на любом этапе появились ошибки, добавляем их в выходной файл
    if (errors.hasErrors()) {
        std::vector<std::string> errorLines = formatter.formatErrors(errors);

        outputLines.insert(
            outputLines.end(),
            errorLines.begin(),
            errorLines.end()
        );
    }

    // создаём отдельный набор ошибок для записи выходного файла
    ErrorSet writeErrors;

    // записываем результат или список ошибок в выходной файл
    fileService.writeLines(outputPath, outputLines, writeErrors);

    // если файл результата не удалось записать, выводим ошибку в консоль
    if (writeErrors.hasErrors()) {
        for (const std::string& line : formatter.formatErrors(writeErrors)) {
            std::cerr << line << std::endl;
        }

        return 1;
    }

    // возвращаем 0 при успешной работе и 1 при наличии ошибок
    return errors.hasErrors() ? 1 : 0;
}