#include "file_service.h"

#include <fstream>
#include <sstream>

std::string FileService::readAll(
    const std::string& path,
    ErrorSet& errors
) const {
    std::ifstream input(path);

    if (!input.is_open()) {
        errors.add(
            ErrorType::FileOpenError,
            "Не удалось открыть входной файл."
        );

        return "";
    }

    std::stringstream buffer;
    buffer << input.rdbuf();

    return buffer.str();
}

void FileService::writeLines(const std::string& path,const std::vector<std::string>& lines,ErrorSet& errors) const {
    std::ofstream output(path);

    if (!output.is_open()) {
        errors.add(
            ErrorType::FileWriteError,
            "Не удалось открыть выходной файл."
        );

        return;
    }

    for (const std::string& line : lines) {
        output << line << '\n';
    }
}