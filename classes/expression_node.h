#ifndef EXPRESSION_NODE_H
#define EXPRESSION_NODE_H

#include "models.h"

enum class NodeType {
    Number,
    Operation
};

struct ExpressionNode {
    NodeType type = NodeType::Number;

    long long value = 0;
    OperationType operation = OperationType::None;

    ExpressionNode* left = nullptr;
    ExpressionNode* right = nullptr;

    explicit ExpressionNode(long long numberValue)
        : type(NodeType::Number),
          value(numberValue),
          operation(OperationType::None),
          left(nullptr),
          right(nullptr) {}

    ExpressionNode(
        OperationType operationValue,
        ExpressionNode* leftNode,
        ExpressionNode* rightNode
    )
        : type(NodeType::Operation),
          value(0),
          operation(operationValue),
          left(leftNode),
          right(rightNode) {}
};

#endif