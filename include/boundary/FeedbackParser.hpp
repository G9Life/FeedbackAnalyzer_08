#pragma once

#include <string>

#include "entity/Types.hpp"

namespace boundary {

struct ParseResult {
    bool success{false};
    std::string body;
    std::string label;
    entity::Status error;
};

class FeedbackParser {
public:
    static ParseResult parsePlainText(const std::string& raw);
    static ParseResult parseLabelBody(const std::string& raw);
};

}  // namespace boundary
