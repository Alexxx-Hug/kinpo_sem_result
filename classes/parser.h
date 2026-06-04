#ifndef PARSER_H
#define PARSER_H

#include "app_error.h"
#include "expression_node.h"
#include "models.h"

#include <memory>
#include <vector>

class Parser {
public:
    ExpressionNode* parse(
        const std::vector<Token>& tokens,
        ErrorSet& errors
    );

private:
    std::vector<Token> tokens;
    size_t position = 0;

    std::vector<std::unique_ptr<ExpressionNode>> allocatedNodes;

    void validateTokens(ErrorSet& errors) const;

    ExpressionNode* parseAddSubExpression();
    ExpressionNode* parseMulDivExpression();
    ExpressionNode* parseFactor();

    ExpressionNode* buildAddSubTree(
        std::vector<ExpressionNode*>& positiveNodes,
        std::vector<ExpressionNode*>& negativeNodes
    );

    ExpressionNode* buildMulDivTree(
        std::vector<ExpressionNode*>& numeratorNodes,
        std::vector<ExpressionNode*>& denominatorNodes
    );

    ExpressionNode* createNumberNode(long long value);

    ExpressionNode* createOperationNode(
        OperationType operation,
        ExpressionNode* left,
        ExpressionNode* right
    );

    const Token& currentToken() const;

    bool match(TokenType type) const;
    bool matchOperation(OperationType operation) const;
    bool isEnd() const;

    void moveNext();
};

#endif