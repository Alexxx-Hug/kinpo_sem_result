#ifndef FILE_SERVICE_H
#define FILE_SERVICE_H

#include "app_error.h"

#include <string>
#include <vector>

/**
 * @brief Класс для работы с файлами
 * 
 * Предоставляет методы для чтения и записи файлов с обработкой ошибок.
 */
class FileService {
public:
    /**
     * @brief Прочитать всё содержимое файла
     * @param path Путь к файлу
     * @param errors Набор ошибок
     * @return Содержимое файла
     */
    std::string readAll(
        const std::string& path,
        ErrorSet& errors
    ) const;

    /**
     * @brief Записать последовательность строк в файл
     * @param path Путь для записи
     * @param lines Вектор строк на запись
     * @param errors Набор ошибок
     */
    void writeLines(
        const std::string& path,
        const std::vector<std::string>& lines,
        ErrorSet& errors
    ) const;
};

#endif