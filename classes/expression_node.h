#ifndef EXPRESSION_NODE_H
#define EXPRESSION_NODE_H

#include "models.h"

/**
 * @brief Тип узла в дереве выражения
 */
enum class NodeType {
    Number,     // Листовой узел с числом
    Operation   // Операционный узел
};

/**
 * @brief Узел абстрактного синтаксического дерева
 * 
 * Представляет либо число (листовой узел), либо бинарную операцию с двумя поддеревьями.
 */
struct ExpressionNode {
    NodeType type = NodeType::Number;       // Тип текущего узла

    long long value = 0;                    // Числовое значение (для Number узлов)
    OperationType operation = OperationType::None; // Тип операции (для Operation узлов)

    ExpressionNode* left = nullptr;         // Левое поддеревье
    ExpressionNode* right = nullptr;        // Правое поддеревье

    /**
     * @brief Конструктор листового узла (число)
     * @param numberValue Числовое значение
     */
    explicit ExpressionNode(long long numberValue)
        : type(NodeType::Number),
          value(numberValue),
          operation(OperationType::None),
          left(nullptr),
          right(nullptr) {}

    /**
     * @brief Конструктор внутреннего узла (операция)
     * @param operationValue Тип операции
     * @param leftNode Левое поддеревье
     * @param rightNode Правое поддеревье
     */
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