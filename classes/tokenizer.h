#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "app_error.h"
#include "models.h"

#include <string>
#include <vector>

class Tokenizer {
public:
    std::vector<Token> tokenize(
        const std::string& expression,
        ErrorSet& errors
    ) const;

private:
    static constexpr int MaxExpressionLength = 100;
    static constexpr long long MaxInputNumber = 500000;

    OperationType operationFromChar(char symbol) const;
};

#endif