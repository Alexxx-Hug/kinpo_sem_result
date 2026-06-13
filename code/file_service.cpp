#include "file_service.h"

#include <fstream>
#include <sstream>

std::string FileService::readAll(const std::string& path,ErrorSet& errors) const {
    // открываем входной файл по переданному пути
    std::ifstream input(path);

    // если файл не открылся, добавляем ошибку и возвращаем пустую строку
    if (!input.is_open()) {
        errors.add(
            ErrorType::FileOpenError,
            "Не удалось открыть входной файл."
        );

        return "";
    }

    // создаём строковый поток для чтения всего содержимого файла
    std::stringstream buffer;

    // переносим содержимое входного файла в строковый поток
    buffer << input.rdbuf();

    // возвращаем считанное содержимое файла как строку
    return buffer.str();
}

void FileService::writeLines(const std::string& path,const std::vector<std::string>& lines,ErrorSet& errors) const {
    // открываем выходной файл в режиме записи с перезаписью старого содержимого
    std::ofstream output(path);

    // если выходной файл не открылся, добавляем ошибку и завершаем функцию
    if (!output.is_open()) {
        errors.add(
            ErrorType::FileWriteError,
            "Не удалось открыть выходной файл."
        );

        return;
    }

    // последовательно записываем каждую строку результата в файл
    for (const std::string& line : lines) {
        output << line << '\n';
    }
}