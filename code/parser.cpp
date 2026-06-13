#include "parser.h"

#include <memory>
#include <vector>

const Token& Parser::currentToken() const {
    // возвращаем токен, на котором сейчас находится парсер
    return tokens[position];
}

bool Parser::match(TokenType type) const {
    // проверяем, совпадает ли тип текущего токена с ожидаемым
    return currentToken().type == type;
}

bool Parser::matchOperation(OperationType operation) const {
    // проверяем, является ли текущий токен операцией нужного типа
    return currentToken().type == TokenType::Operation &&
           currentToken().operation == operation;
}

bool Parser::isEnd() const {
    // проверяем, дошёл ли парсер до служебного токена конца выражения
    return currentToken().type == TokenType::End;
}

void Parser::moveNext() {
    // сдвигаем позицию на следующий токен, если он существует
    if (position + 1 < tokens.size()) {
        ++position;
    }
}

ExpressionNode* Parser::createNumberNode(long long value) {
    // создаём узел числа и сохраняем его во внутреннем хранилище
    allocatedNodes.push_back(std::make_unique<ExpressionNode>(value));

    // возвращаем обычный указатель на созданный узел
    return allocatedNodes.back().get();
}

ExpressionNode* Parser::createOperationNode(OperationType operation, ExpressionNode* left, ExpressionNode* right) {
    // создаём узел операции с левым и правым потомком
    allocatedNodes.push_back(
        std::make_unique<ExpressionNode>(operation, left, right)
    );

    // возвращаем обычный указатель на созданный узел
    return allocatedNodes.back().get();
}

void Parser::validateTokens(ErrorSet& errors) const {
    // если список токенов пустой, выражение невозможно разобрать
    if (tokens.empty()) {
        errors.add(
            ErrorType::SyntaxError,
            "Во входном выражении обнаружена синтаксическая ошибка."
        );

        return;
    }

    // переменная хранит текущий баланс открывающих и закрывающих скобок
    int balance = 0;

    // последовательно проверяем каждый токен выражения
    for (size_t i = 0; i < tokens.size(); ++i) {
        // берём текущий токен для проверки
        const Token& current = tokens[i];

        // служебный токен End означает конец полезной части выражения
        if (current.type == TokenType::End) {
            break;
        }

        // обрабатываем открывающую скобку
        if (current.type == TokenType::LeftBracket) {
            // увеличиваем баланс, так как встретили новую открывающую скобку
            ++balance;

            // проверяем случай пустых скобок
            if (i + 1 < tokens.size() && tokens[i + 1].type == TokenType::RightBracket) {
                errors.add(
                    ErrorType::EmptyBrackets,
                    "Во входном выражении обнаружены пустые скобки. Между скобками должно находиться выражение."
                );
            }

            // после открывающей скобки не должна сразу идти операция
            if (i + 1 < tokens.size() && tokens[i + 1].type == TokenType::Operation) {
                errors.add(
                    ErrorType::MissingOperand,
                    "Подвыражение в скобках записано некорректно. После открывающей скобки должен следовать операнд."
                );
            }

            // перед открывающей скобкой не должно быть числа без знака операции
            if (i > 0 && tokens[i - 1].type == TokenType::Number) {
                errors.add(
                    ErrorType::MissingOperation,
                    "Между числом и открывающей скобкой отсутствует знак операции. Проверьте запись выражения."
                );
            }

            // между закрывающей и открывающей скобкой должен быть знак операции
            if (i > 0 && tokens[i - 1].type == TokenType::RightBracket) {
                errors.add(
                    ErrorType::MissingOperation,
                    "Между соседними подвыражениями отсутствует знак операции. Проверьте запись выражения."
                );
            }
        }

        // обрабатываем закрывающую скобку
        if (current.type == TokenType::RightBracket) {
            // уменьшаем баланс, так как одна скобочная группа закрылась
            --balance;

            // отрицательный баланс означает лишнюю закрывающую скобку
            if (balance < 0) {
                errors.add(
                    ErrorType::BracketBalanceError,
                    "Во входном выражении обнаружена закрывающая скобка без соответствующей открывающей. Проверьте порядок скобок."
                );
            }

            // перед закрывающей скобкой не должна стоять операция
            if (i > 0 && tokens[i - 1].type == TokenType::Operation) {
                errors.add(
                    ErrorType::MissingOperand,
                    "Подвыражение в скобках записано некорректно. Перед закрывающей скобкой не может находиться знак операции."
                );
            }

            // после закрывающей скобки не должно сразу идти число
            if (i + 1 < tokens.size() && tokens[i + 1].type == TokenType::Number) {
                errors.add(
                    ErrorType::MissingOperation,
                    "Между закрывающей скобкой и числом отсутствует знак операции. Проверьте запись выражения."
                );
            }

            // после закрывающей скобки не должна сразу идти открывающая скобка
            if (i + 1 < tokens.size() && tokens[i + 1].type == TokenType::LeftBracket) {
                errors.add(
                    ErrorType::MissingOperation,
                    "Между соседними подвыражениями отсутствует знак операции. Проверьте запись выражения."
                );
            }
        }

        // обрабатываем числовой токен
        if (current.type == TokenType::Number) {
            // два числа подряд означают неверную запись выражения
            if (i + 1 < tokens.size() && tokens[i + 1].type == TokenType::Number) {
                errors.add(
                    ErrorType::SyntaxError,
                    "Число записано некорректно. Цифры одного числа не должны разделяться пробелами."
                );
            }

            // после числа не должна сразу идти открывающая скобка
            if (i + 1 < tokens.size() && tokens[i + 1].type == TokenType::LeftBracket) {
                errors.add(
                    ErrorType::MissingOperation,
                    "Между числом и открывающей скобкой отсутствует знак операции. Проверьте запись выражения."
                );
            }
        }

        // обрабатываем токен арифметической операции
        if (current.type == TokenType::Operation) {
            // выражение не может начинаться с операции
            if (i == 0) {
                // отдельно обрабатываем минус в начале как попытку записать отрицательное число
                if (current.operation == OperationType::Subtract) {
                    errors.add(
                        ErrorType::NegativeInputNumber,
                        "Во входном выражении обнаружено отрицательное число. Допустимы только положительные целые числа."
                    );
                } else {
                    errors.add(
                        ErrorType::SyntaxError,
                        "Во входном выражении обнаружена синтаксическая ошибка. Выражение не может начинаться со знака операции."
                    );
                }
            }

            // выражение не может заканчиваться операцией
            if (i + 1 >= tokens.size() || tokens[i + 1].type == TokenType::End) {
                errors.add(
                    ErrorType::MissingOperand,
                    "Выражение не может заканчиваться знаком операции. После операции должен идти операнд."
                );
            }

            // две операции подряд недопустимы
            if (i + 1 < tokens.size() && tokens[i + 1].type == TokenType::Operation) {
                errors.add(
                    ErrorType::SyntaxError,
                    "Во входном выражении обнаружена синтаксическая ошибка. В записи не может идти две операции подряд."
                );
            }

            // перед закрывающей скобкой не должна стоять операция
            if (i + 1 < tokens.size() && tokens[i + 1].type == TokenType::RightBracket) {
                errors.add(
                    ErrorType::MissingOperand,
                    "Подвыражение в скобках записано некорректно. Перед закрывающей скобкой не может находиться знак операции."
                );
            }
        }
    }

    // ненулевой баланс означает нарушение количества открывающих и закрывающих скобок
    if (balance != 0) {
        errors.add(
            ErrorType::BracketBalanceError,
            "Во входном выражении нарушен баланс скобок. Количество открывающих и закрывающих скобок должно совпадать."
        );
    }
}

ExpressionNode* Parser::parseFactor() {
    // если текущий токен является числом, создаём лист дерева
    if (match(TokenType::Number)) {
        long long value = currentToken().value;

        // переходим к следующему токену после числа
        moveNext();

        return createNumberNode(value);
    }

    // если текущий токен является открывающей скобкой, разбираем вложенное выражение
    if (match(TokenType::LeftBracket)) {
        // пропускаем открывающую скобку
        moveNext();

        // разбираем выражение внутри скобок
        ExpressionNode* node = parseAddSubExpression();

        // если после вложенного выражения стоит закрывающая скобка, пропускаем её
        if (match(TokenType::RightBracket)) {
            moveNext();
        }

        return node;
    }

    // возвращаем технический узел, если фактор не удалось распознать
    return createNumberNode(1);
}

ExpressionNode* Parser::parseMulDivExpression() {
    // список множителей, которые будут объединены операциями умножения
    std::vector<ExpressionNode*> numeratorNodes;

    // список делителей, которые будут присоединены операциями деления
    std::vector<ExpressionNode*> denominatorNodes;

    // первый фактор всегда относится к числителю
    numeratorNodes.push_back(parseFactor());

    // пока встречаются операции умножения или деления, продолжаем разбор
    while (
        matchOperation(OperationType::Multiply) ||
        matchOperation(OperationType::Divide)
    ) {
        // запоминаем текущую операцию
        OperationType operation = currentToken().operation;

        // переходим к следующему токену после знака операции
        moveNext();

        // разбираем следующий фактор
        ExpressionNode* nextNode = parseFactor();

        // множители добавляем в список числителя
        if (operation == OperationType::Multiply) {
            numeratorNodes.push_back(nextNode);
        } else {
            // делители добавляем в список знаменателя
            denominatorNodes.push_back(nextNode);
        }
    }

    // строим безопасное дерево умножения и деления
    return buildMulDivTree(numeratorNodes, denominatorNodes);
}

ExpressionNode* Parser::buildMulDivTree(std::vector<ExpressionNode*>& numeratorNodes,std::vector<ExpressionNode*>& denominatorNodes) {
    // начинаем построение дерева с первого множителя
    ExpressionNode* current = numeratorNodes[0];

    // последовательно объединяем все множители операцией умножения
    for (size_t i = 1; i < numeratorNodes.size(); ++i) {
        current = createOperationNode(
            OperationType::Multiply,
            current,
            numeratorNodes[i]
        );
    }

    // после умножений последовательно присоединяем делители
    for (ExpressionNode* denominatorNode : denominatorNodes) {
        current = createOperationNode(
            OperationType::Divide,
            current,
            denominatorNode
        );
    }

    // возвращаем корень построенного поддерева
    return current;
}

ExpressionNode* Parser::parseAddSubExpression() {
    // список положительных частей выражения
    std::vector<ExpressionNode*> positiveNodes;

    // список частей выражения, которые нужно вычесть
    std::vector<ExpressionNode*> negativeNodes;

    // первая часть выражения всегда считается положительной
    positiveNodes.push_back(parseMulDivExpression());

    // пока встречаются операции сложения или вычитания, продолжаем разбор
    while (
        matchOperation(OperationType::Add) ||
        matchOperation(OperationType::Subtract)
    ) {
        // запоминаем текущую операцию
        OperationType operation = currentToken().operation;

        // переходим к следующему токену после знака операции
        moveNext();

        // разбираем следующую часть выражения с учётом приоритета умножения и деления
        ExpressionNode* nextNode = parseMulDivExpression();

        // слагаемые добавляем в список положительных узлов
        if (operation == OperationType::Add) {
            positiveNodes.push_back(nextNode);
        } else {
            // вычитаемые части добавляем в список отрицательных узлов
            negativeNodes.push_back(nextNode);
        }
    }

    // строим безопасное дерево сложения и вычитания
    return buildAddSubTree(positiveNodes, negativeNodes);
}

ExpressionNode* Parser::buildAddSubTree(std::vector<ExpressionNode*>& positiveNodes,std::vector<ExpressionNode*>& negativeNodes) {
    // начинаем построение дерева с первой положительной части
    ExpressionNode* current = positiveNodes[0];

    // сначала объединяем все положительные части через сложение
    for (size_t i = 1; i < positiveNodes.size(); ++i) {
        current = createOperationNode(
            OperationType::Add,
            current,
            positiveNodes[i]
        );
    }

    // затем последовательно присоединяем вычитаемые части
    for (ExpressionNode* negativeNode : negativeNodes) {
        current = createOperationNode(
            OperationType::Subtract,
            current,
            negativeNode
        );
    }

    // возвращаем корень построенного поддерева
    return current;
}

ExpressionNode* Parser::parse(const std::vector<Token>& sourceTokens,ErrorSet& errors) {
    // сохраняем полученные токены во внутреннее поле парсера
    tokens = sourceTokens;

    // начинаем разбор с первого токена
    position = 0;

    // очищаем старые узлы перед новым построением дерева
    allocatedNodes.clear();

    // перед построением дерева проверяем корректность порядка токенов
    validateTokens(errors);

    // если проверка нашла ошибки, дерево не строится
    if (errors.hasErrors()) {
        return nullptr;
    }

    // запускаем разбор выражения с самого низкого приоритета операций
    ExpressionNode* root = parseAddSubExpression();

    // если после разбора остались лишние токены, фиксируем ошибку
    if (!isEnd()) {
        // отдельная проверка лишней закрывающей скобки
        if (match(TokenType::RightBracket)) {
            errors.add(
                ErrorType::BracketBalanceError,
                "Во входном выражении обнаружена закрывающая скобка без соответствующей открывающей. Проверьте порядок скобок."
            );

            return nullptr;
        }

        // если остался другой токен, считаем это общей синтаксической ошибкой
        errors.add(
            ErrorType::SyntaxError,
            "Во входном выражении обнаружена синтаксическая ошибка."
        );

        return nullptr;
    }

    // возвращаем корень построенного дерева
    return root;
}