#pragma once

#include <string>
#include <vector>

#include "entity/Types.hpp"

namespace boundary {

struct CsvRenderResult {
    bool success{false};
    std::string body;
    int httpStatus{200};
    entity::Status error;
};

class CsvPresenter {
public:
    static CsvRenderResult render(const std::vector<entity::Feedback>& filtered);
};

}  // namespace boundary
