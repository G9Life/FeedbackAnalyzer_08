#pragma once

#include <string>

#include "entity/Types.hpp"

namespace boundary {

struct CsvValidationResult {
    bool success{false};
    entity::Status error;
};

class CsvUploadValidator {
public:
    static CsvValidationResult validate(const std::string& csvContent);
};

}  // namespace boundary
