#include "tokenizer.h"

#include <cctype>

#include <string>

#include <vector>

OperationType Tokenizer::operationFromChar(char symbol) const {

    if (symbol == '+') {

        return OperationType::Add;

    }

    if (symbol == '-') {

        return OperationType::Subtract;

    }

    if (symbol == '*') {

        return OperationType::Multiply;

    }

    if (symbol == '/') {

        return OperationType::Divide;

    }

    return OperationType::None;

}