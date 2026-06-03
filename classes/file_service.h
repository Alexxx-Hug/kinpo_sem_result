#ifndef FILE_SERVICE_H
#define FILE_SERVICE_H

#include "app_error.h"

#include <string>
#include <vector>

class FileService {
public:
    std::string readAll(
        const std::string& path,
        ErrorSet& errors
    ) const;

    void writeLines(
        const std::string& path,
        const std::vector<std::string>& lines,
        ErrorSet& errors
    ) const;
};

#endif