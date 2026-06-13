#ifndef APP_ERROR_H
#define APP_ERROR_H

#include <set>
#include <string>

/**
 * @brief Тип ошибки, возникшей во время работы программы.
 */
enum class ErrorType {
    FileOpenError,
    FileWriteError,

    EmptyInput,
    OnlySpacesInput,
    InvalidSymbol,
    NotIntegerNumber,
    NegativeInputNumber,
    ZeroOperand,
    NumberOutOfRange,
    ExpressionTooLong,

    SyntaxError,
    BracketBalanceError,
    EmptyBrackets,
    MissingOperation,
    MissingOperand,

    DivisionByZero,
    NegativeResult,
    FractionResult,
    IntermediateOverflow,
    UnknownOperation,

    InvalidArgumentCount,
    UnknownError
};

/**
 * @brief Представляет одну ошибку приложения.
 *
 * Хранит тип ошибки и её текстовое описание.
 */
class AppError {
public:

    /**
     * @brief Создаёт объект ошибки.
     *
     * @param errorType Тип ошибки.
     * @param errorMessage Текстовое описание ошибки.
     */
    AppError(
        ErrorType errorType,
        const std::string& errorMessage
    )
        : type(errorType),
          message(errorMessage) {
    }

    /**
     * @brief Возвращает тип ошибки.
     *
     * @return Тип ошибки.
     */
    ErrorType getType() const {
        return type;
    }

    /**
     * @brief Возвращает сообщение об ошибке.
     *
     * @return Текст ошибки.
     */
    std::string getMessage() const {
        return message;
    }

    /**
     * @brief Сравнивает ошибки для хранения в std::set.
     *
     * @param other Объект для сравнения.
     * @return true, если текущая ошибка меньше другой.
     */
    bool operator<(const AppError& other) const {
        if (type != other.type) {
            return type < other.type;
        }

        return message < other.message;
    }

private:

    ErrorType type;      ///< Тип ошибки.
    std::string message; ///< Текст ошибки.
};

/**
 * @brief Контейнер для хранения ошибок приложения.
 *
 * Использует std::set для исключения дубликатов
 * и обеспечения стабильного порядка хранения.
 */
class ErrorSet {
public:

    /**
     * @brief Добавляет ошибку в набор.
     *
     * @param type Тип ошибки.
     * @param message Текст ошибки.
     */
    void add(
        ErrorType type,
        const std::string& message
    ) {
        errors.insert(AppError(type, message));
    }

    /**
     * @brief Проверяет наличие ошибок.
     *
     * @return true, если набор не пуст.
     */
    bool hasErrors() const {
        return !errors.empty();
    }

    /**
     * @brief Возвращает набор ошибок.
     *
     * @return Константная ссылка на множество ошибок.
     */
    const std::set<AppError>& getErrors() const {
        return errors;
    }

    /**
     * @brief Удаляет все ошибки из набора.
     */
    void clear() {
        errors.clear();
    }

private:

    std::set<AppError> errors; ///< Набор зарегистрированных ошибок.
};

#endif