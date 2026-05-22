#include "boundary/CsvPresenter.hpp"
#include "boundary/CsvUploadValidator.hpp"
#include "boundary/FeedbackParser.hpp"
#include "boundary/FilterValidator.hpp"

#include <algorithm>
#include <set>
#include <sstream>
#include <string>

namespace {

std::string trim(const std::string& raw) {
    const auto start = raw.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    }
    const auto end = raw.find_last_not_of(" \t\r\n");
    return raw.substr(start, end - start + 1);
}

bool isWhitespaceOnly(const std::string& s) {
    return trim(s).empty();
}

}  // namespace

namespace boundary {

ParseResult FeedbackParser::parsePlainText(const std::string& raw) {
    const std::string body = trim(raw);
    if (body.empty()) {
        return {false, {}, {}, entity::Status::Fail("EMPTY_TEXT", "text")};
    }
    return {true, body, {}, entity::Status::Ok()};
}

ParseResult FeedbackParser::parseLabelBody(const std::string& raw) {
    const auto colon = raw.find(':');
    if (colon == std::string::npos) {
        return {false, {}, {}, entity::Status::Fail("COLON_MISSING", "text")};
    }
    const std::string label = trim(raw.substr(0, colon));
    const std::string body = trim(raw.substr(colon + 1));
    if (label.empty()) {
        return {false, {}, {}, entity::Status::Fail("EMPTY_LABEL", "text")};
    }
    if (body.empty()) {
        return {false, {}, {}, entity::Status::Fail("EMPTY_BODY", "text")};
    }
    const std::set<std::string> allowed = {"긍정", "부정", "중립"};
    if (!allowed.count(label)) {
        return {false, {}, {}, entity::Status::Fail("UNKNOWN_EMOTION", "text", label)};
    }
    return {true, body, label, entity::Status::Ok()};
}

FilterValidator::FilterValidator(entity::UnitRegistry registry)
    : registry_(std::move(registry)) {}

entity::Status FilterValidator::validate(const std::string& sentimentFilter,
                                       const std::string& categoryFilter) const {
    const std::string sent = trim(sentimentFilter);
    const std::string cat = trim(categoryFilter);
    if (isWhitespaceOnly(sent)) {
        return entity::Status::Fail("UNKNOWN_SENTIMENT_FILTER", "sentiment");
    }
    const std::set<std::string> allowedSent = {"전체", "긍정", "부정", "중립"};
    if (!allowedSent.count(sent) || (sent != "전체" && !registry_.hasSentiment(sent))) {
        return entity::Status::Fail("UNKNOWN_SENTIMENT_FILTER", "sentiment");
    }
    if (cat != "전체" && !registry_.hasCategory(cat)) {
        return entity::Status::Fail("UNKNOWN_CATEGORY_FILTER", "keyword");
    }
    return entity::Status::Ok();
}

CsvValidationResult CsvUploadValidator::validate(const std::string& csvContent) {
    if (csvContent.empty()) {
        return {false, entity::Status::Fail("CSV_MISSING_TEXT_COLUMN", "file")};
    }
    const auto lineEnd = csvContent.find('\n');
    const std::string header =
        lineEnd == std::string::npos ? csvContent : csvContent.substr(0, lineEnd);
    if (header.find("text") == std::string::npos) {
        return {false, entity::Status::Fail("CSV_MISSING_TEXT_COLUMN", "file")};
    }
    return {true, entity::Status::Ok()};
}

CsvRenderResult CsvPresenter::render(const std::vector<entity::Feedback>& filtered) {
    if (filtered.empty()) {
        return {false, {}, 404, entity::Status::Fail("NO_DATA_TO_EXPORT", "")};
    }
    std::ostringstream oss;
    oss << "text\n";
    for (const auto& fb : filtered) {
        oss << fb.text << '\n';
    }
    return {true, oss.str(), 200, entity::Status::Ok()};
}

}  // namespace boundary
