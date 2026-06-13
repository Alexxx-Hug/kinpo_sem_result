#include "step_formatter.h"

#include <sstream>
#include <string>
#include <vector>

char StepFormatter::operationToChar(OperationType operation) const {
    // преобразуем операцию сложения в символ
    if (operation == OperationType::Add) {
        return '+';
    }

    // преобразуем операцию вычитания в символ
    if (operation == OperationType::Subtract) {
        return '-';
    }

    // преобразуем операцию умножения в символ
    if (operation == OperationType::Multiply) {
        return '*';
    }

    // преобразуем операцию деления в символ
    if (operation == OperationType::Divide) {
        return '/';
    }

    // возвращаем знак вопроса, если операция неизвестна
    return '?';
}

std::vector<std::string> StepFormatter::formatSteps(const std::vector<CalculationStep>& steps,long long finalResult) const {
    // создаём список строк для записи в выходной файл
    std::vector<std::string> lines;

    // если шагов нет, значит выражение состояло из одного числа
    if (steps.empty()) {
        lines.push_back("1) " + std::to_string(finalResult));
        return lines;
    }

    // проходим по всем шагам вычисления
    for (size_t i = 0; i < steps.size(); ++i) {
        // берём текущий шаг вычисления
        const CalculationStep& step = steps[i];

        // создаём поток для удобного формирования строки
        std::ostringstream output;

        // формируем строку вида: 1) 2 + 3 = 5
        output << i + 1 << ") "
               << step.left
               << " "
               << operationToChar(step.operation)
               << " "
               << step.right
               << " = "
               << step.result;

        // добавляем готовую строку в итоговый список
        lines.push_back(output.str());
    }

    // возвращаем список строк с пошаговым решением
    return lines;
}

std::vector<std::string> StepFormatter::formatErrors(const ErrorSet& errors) const {
    // создаём список строк для сообщений об ошибках
    std::vector<std::string> lines;

    // проходим по всем ошибкам из ErrorSet
    for (const AppError& error : errors.getErrors()) {
        // добавляем текст ошибки в формате для вывода
        lines.push_back("Ошибка: " + error.getMessage());
    }

    // возвращаем список строк с ошибками
    return lines;
}