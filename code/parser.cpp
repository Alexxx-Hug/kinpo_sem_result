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