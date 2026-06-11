#ifndef APP_ERROR_H
#define APP_ERROR_H

#include <string>
#include <vector>

/**
 * @brief Тип ошибки в приложении
 */
enum class ErrorType {
    FileOpenError,       // Ошибка открытия файла
    FileWriteError,      // Ошибка записи в файл

    EmptyInput,          // Пустые данные
    OnlySpacesInput,     // Выражение содержит только пробелы
    InvalidSymbol,       // Недопустимый символ
    NotIntegerNumber,    // Число не является целым
    NegativeInputNumber, // Отрицательное число на входе
    ZeroOperand,         // Нуль как операнд
    NumberOutOfRange,    // Число выходит за пределы
    ExpressionTooLong,   // Выражение слишком длинное

    UnknownOperation,    // Неизвестная операция
    SyntaxError,         // Ошибка синтаксиса
    BracketBalanceError, // Несовпадение скобок
    EmptyBrackets,       // Пустые скобки
    MissingOperation,    // Отсутствует операция
    MissingOperand,      // Отсутствует операнд

    DivisionByZero,      // Деление на ноль
    NegativeResult,      // Отрицательный результат
    FractionResult,      // Не целочисленный результат
    IntermediateOverflow, // Промежуточные вычисления превысили порог

    InvalidArgumentCount, // Неверное количество аргументов командной строки
    UnknownError         // Неизвестная ошибка
};

/**
 * @brief Класс для представления одной ошибки
 * 
 * Хранит тип ошибки и дескриптивное сообщение.
 */
class AppError {
public:
    /**
     * @brief Конструктор ошибки
     * @param errorType Тип ошибки
     * @param errorMessage Описание ошибки
     */
    AppError(ErrorType errorType, const std::string& errorMessage)
        : type(errorType), message(errorMessage) {}

    /**
     * @brief Получить тип ошибки
     * @return Настоящий тип ошибки
     */
    ErrorType getType() const {
        return type;
    }

    /**
     * @brief Получить текст ошибки
     * @return Описание ошибки
     */
    std::string getMessage() const {
        return message;
    }

private:
    ErrorType type;
    std::string message;
};

/**
 * @brief Класс для управления набором ошибок
 * 
 * Обеспечивает сбор и отчётность ошибок, группировку нескольких ошибок.
 */
class ErrorSet {
public:
    /**
     * @brief Добавить новую ошибку в набор
     * @param type Тип ошибки
     * @param message Описание ошибки
     */
    void add(ErrorType type, const std::string& message) {
        errors.emplace_back(type, message);
    }

    /**
     * @brief Проверить, есть ли ошибки
     * @return true если набор не пуст, false иначе
     */
    bool hasErrors() const {
        return !errors.empty();
    }

    /**
     * @brief Получить вектор всех ошибок
     * @return Константная ссылка на вектор ошибок
     */
    const std::vector<AppError>& getErrors() const {
        return errors;
    }

    /**
     * @brief Очистить все ошибки
     */
    void clear() {
        errors.clear();
    }

private:
    std::vector<AppError> errors;
};

#endif