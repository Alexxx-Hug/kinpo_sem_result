#include "parser.h"

#include <memory>
#include <vector>

const Token& Parser::currentToken() const {
    return tokens[position];
}

bool Parser::match(TokenType type) const {
    return currentToken().type == type;
}

bool Parser::matchOperation(OperationType operation) const {
    return currentToken().type == TokenType::Operation &&
           currentToken().operation == operation;
}

bool Parser::isEnd() const {
    return currentToken().type == TokenType::End;
}

void Parser::moveNext() {
    if (position + 1 < tokens.size()) {
        ++position;
    }
}

ExpressionNode* Parser::createNumberNode(long long value) {
    allocatedNodes.push_back(std::make_unique<ExpressionNode>(value));
    return allocatedNodes.back().get();
}

ExpressionNode* Parser::createOperationNode(OperationType operation, ExpressionNode* left, ExpressionNode* right) {
    allocatedNodes.push_back(
        std::make_unique<ExpressionNode>(operation, left, right)
    );

    return allocatedNodes.back().get();
}

void Parser::validateTokens(ErrorSet& errors) const {
    if (tokens.empty()) {
        errors.add(
            ErrorType::SyntaxError,
            "Во входном выражении обнаружена синтаксическая ошибка."
        );
        return;
    }
    int balance = 0;
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        const Token& current = tokens[i];
        if (current.type == TokenType::End) {
            break;
        }
        if (current.type == TokenType::LeftBracket) {
            ++balance;
            if (i + 1 < tokens.size() && tokens[i + 1].type == TokenType::RightBracket) {
                errors.add(
                    ErrorType::EmptyBrackets,
                    "Во входном выражении обнаружены пустые скобки. Между скобками должно находиться выражение."
                );
            }
            if (i + 1 < tokens.size() && tokens[i + 1].type == TokenType::Operation) {
                errors.add(
                    ErrorType::MissingOperand,
                    "Подвыражение в скобках записано некорректно. После открывающей скобки должен следовать операнд."
                );
            }
            if (i > 0 && tokens[i - 1].type == TokenType::Number) {
                errors.add(
                    ErrorType::MissingOperation,
                    "Между числом и открывающей скобкой отсутствует знак операции. Проверьте запись выражения."
                );
            }
            if (i > 0 && tokens[i - 1].type == TokenType::RightBracket) {
                errors.add(
                    ErrorType::MissingOperation,
                    "Между соседними подвыражениями отсутствует знак операции. Проверьте запись выражения."
                );
            }
        }
        if (current.type == TokenType::RightBracket) {
            --balance;
            if (balance < 0) {
                errors.add(
                    ErrorType::BracketBalanceError,
                    "Во входном выражении обнаружена закрывающая скобка без соответствующей открывающей. Проверьте порядок скобок."
                );
            }
            if (i > 0 && tokens[i - 1].type == TokenType::Operation) {
                errors.add(
                    ErrorType::MissingOperand,
                    "Подвыражение в скобках записано некорректно. Перед закрывающей скобкой не может находиться знак операции."
                );
            }
            if (i + 1 < tokens.size() && tokens[i + 1].type == TokenType::Number) {
                errors.add(
                    ErrorType::MissingOperation,
                    "Между закрывающей скобкой и числом отсутствует знак операции. Проверьте запись выражения."
                );
            }
            if (i + 1 < tokens.size() && tokens[i + 1].type == TokenType::LeftBracket) {
                errors.add(
                    ErrorType::MissingOperation,
                    "Между соседними подвыражениями отсутствует знак операции. Проверьте запись выражения."
                );
            }
        }
        if (current.type == TokenType::Number) {
            if (i + 1 < tokens.size() && tokens[i + 1].type == TokenType::Number) {
                errors.add(
                    ErrorType::SyntaxError,
                    "Число записано некорректно. Цифры одного числа не должны разделяться пробелами."
                );
            }

            if (i + 1 < tokens.size() && tokens[i + 1].type == TokenType::LeftBracket) {
                errors.add(
                    ErrorType::MissingOperation,
                    "Между числом и открывающей скобкой отсутствует знак операции. Проверьте запись выражения."
                );
            }
        }
        if (current.type == TokenType::Operation) {
            if (i == 0) {
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
            if (i + 1 >= tokens.size() || tokens[i + 1].type == TokenType::End) {
                errors.add(
                    ErrorType::MissingOperand,
                    "Выражение не может заканчиваться знаком операции. После операции должен идти операнд."
                );
            }
            if (i + 1 < tokens.size() && tokens[i + 1].type == TokenType::Operation) {
                errors.add(
                    ErrorType::SyntaxError,
                    "Во входном выражении обнаружена синтаксическая ошибка. В записи не может идти две операции подряд."
                );
            }
            if (i + 1 < tokens.size() && tokens[i + 1].type == TokenType::RightBracket) {
                errors.add(
                    ErrorType::MissingOperand,
                    "Подвыражение в скобках записано некорректно. Перед закрывающей скобкой не может находиться знак операции."
                );
            }
        }
    }
    if (balance != 0) {
        errors.add(
            ErrorType::BracketBalanceError,
            "Во входном выражении нарушен баланс скобок. Количество открывающих и закрывающих скобок должно совпадать."
        );
    }
}

ExpressionNode* Parser::parseFactor() {
    if (match(TokenType::Number)) {
        long long value = currentToken().value;
        moveNext();

        return createNumberNode(value);
    }

    if (match(TokenType::LeftBracket)) {
        moveNext();

        ExpressionNode* node = parseAddSubExpression();

        if (match(TokenType::RightBracket)) {
            moveNext();
        }

        return node;
    }

    return createNumberNode(1);
}

ExpressionNode* Parser::parseMulDivExpression() {
    std::vector<ExpressionNode*> numeratorNodes;
    std::vector<ExpressionNode*> denominatorNodes;

    numeratorNodes.push_back(parseFactor());

    while (
        matchOperation(OperationType::Multiply) ||
        matchOperation(OperationType::Divide)
    ) {
        OperationType operation = currentToken().operation;
        moveNext();

        ExpressionNode* nextNode = parseFactor();

        if (operation == OperationType::Multiply) {
            numeratorNodes.push_back(nextNode);
        } else {
            denominatorNodes.push_back(nextNode);
        }
    }

    return buildMulDivTree(numeratorNodes, denominatorNodes);
}

ExpressionNode* Parser::buildMulDivTree(
    std::vector<ExpressionNode*>& numeratorNodes,
    std::vector<ExpressionNode*>& denominatorNodes
) {
    ExpressionNode* current = numeratorNodes[0];

    for (size_t i = 1; i < numeratorNodes.size(); ++i) {
        current = createOperationNode(
            OperationType::Multiply,
            current,
            numeratorNodes[i]
        );
    }

    for (ExpressionNode* denominatorNode : denominatorNodes) {
        current = createOperationNode(
            OperationType::Divide,
            current,
            denominatorNode
        );
    }

    return current;
}

ExpressionNode* Parser::parseAddSubExpression() {
    std::vector<ExpressionNode*> positiveNodes;
    std::vector<ExpressionNode*> negativeNodes;

    positiveNodes.push_back(parseMulDivExpression());

    while (
        matchOperation(OperationType::Add) ||
        matchOperation(OperationType::Subtract)
    ) {
        OperationType operation = currentToken().operation;
        moveNext();

        ExpressionNode* nextNode = parseMulDivExpression();

        if (operation == OperationType::Add) {
            positiveNodes.push_back(nextNode);
        } else {
            negativeNodes.push_back(nextNode);
        }
    }

    return buildAddSubTree(positiveNodes, negativeNodes);
}

ExpressionNode* Parser::buildAddSubTree(
    std::vector<ExpressionNode*>& positiveNodes,
    std::vector<ExpressionNode*>& negativeNodes
) {
    ExpressionNode* current = positiveNodes[0];

    for (size_t i = 1; i < positiveNodes.size(); ++i) {
        current = createOperationNode(
            OperationType::Add,
            current,
            positiveNodes[i]
        );
    }

    for (ExpressionNode* negativeNode : negativeNodes) {
        current = createOperationNode(
            OperationType::Subtract,
            current,
            negativeNode
        );
    }

    return current;
}