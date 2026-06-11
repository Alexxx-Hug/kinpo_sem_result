#ifndef PARSER_H
#define PARSER_H

#include "app_error.h"
#include "expression_node.h"
#include "models.h"

#include <memory>
#include <vector>

/**
 * @brief Класс для разбора выражения
 * 
 * Преобразует последовательность токенов в абстрактное синтактическое дерево.
 */
class Parser {
public:
    /**
     * @brief Основной метод для разбора токенов
     * @param tokens Последовательность токенов на входе
     * @param errors Набор ошибок об отклонениях
     * @return Корень построенного синтаксического дерева
     */
    ExpressionNode* parse(
        const std::vector<Token>& tokens,
        ErrorSet& errors
    );

private:
    std::vector<Token> tokens; // Токены для разбора
    size_t position = 0; // Текущая позиция в токенах

    std::vector<std::unique_ptr<ExpressionNode>> allocatedNodes; // Коллекция всех созданных узлов дерева

    /**
     * @brief Проверить последовательность токенов на валидность
     * @param errors Набор ошибок
     */
    void validateTokens(ErrorSet& errors) const;

    /**
     * @brief Разбор выражения для сложения и вычитания
     * @return Корень дерева выражения
     */
    ExpressionNode* parseAddSubExpression();
    /**
     * @brief Разбор выражения для умножения и деления
     * @return Корень дерева выражения
     */
    ExpressionNode* parseMulDivExpression();
    /**
     * @brief Разбор основного элемента выражения
     * @return Узел выражения
     */
    ExpressionNode* parseFactor();

    /**
     * @brief Построить дерево для сложения/вычитания
     * @param positiveNodes Операнды для сложения
     * @param negativeNodes Операнды для вычитания
     * @return Готовое дерево выражения
     */
    ExpressionNode* buildAddSubTree(
        std::vector<ExpressionNode*>& positiveNodes,
        std::vector<ExpressionNode*>& negativeNodes
    );

    /**
     * @brief Построить дерево для умножения/деления
     * @param numeratorNodes Операнды для умножения
     * @param denominatorNodes Операнды для деления
     * @return Готовое дерево выражения
     */
    ExpressionNode* buildMulDivTree(
        std::vector<ExpressionNode*>& numeratorNodes,
        std::vector<ExpressionNode*>& denominatorNodes
    );

    /**
     * @brief Создать узел для числа
     * @param value Числовое значение
     * @return Новый узел выражения
     */
    ExpressionNode* createNumberNode(long long value);

    /**
     * @brief Создать узел для операции
     * @param operation Тип операции
     * @param left Левый узел
     * @param right Правый узел
     * @return Новый узел бинарной операции
     */
    ExpressionNode* createOperationNode(
        OperationType operation,
        ExpressionNode* left,
        ExpressionNode* right
    );

    /**
     * @brief Получить токен на текущей позиции
     * @return Текущий токен
     */
    const Token& currentToken() const;

    /**
     * @brief Проверить, соответствует ли текущий токен типу
     * @param type Ожидаемый тип
     * @return true если типы совпадают
     */
    bool match(TokenType type) const;
    /**
     * @brief Проверить, является ли текущая операция требуемой
     * @param operation Ожидаемая операция
     * @return true если операции совпадают
     */
    bool matchOperation(OperationType operation) const;
    /**
     * @brief Проверить, достигном конец последовательности
     * @return true если последовательность окончена
     */
    bool isEnd() const;

    /**
     * @brief Переместиться к следующему токену
     */
    void moveNext();
};

#endif