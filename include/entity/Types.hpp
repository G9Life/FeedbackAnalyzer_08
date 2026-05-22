#pragma once

#include <map>
#include <string>
#include <vector>

namespace entity {

struct Feedback {
    std::string text;
    explicit Feedback(std::string t) : text(std::move(t)) {}
};

struct AnalysisResult {
    std::string sentimentId;
    std::vector<std::string> categoryIds;
};

struct AggregateSnapshot {
    std::map<std::string, int> sentimentCounts;
    std::map<std::string, int> categoryCounts;
};

struct Status {
    bool ok{false};
    std::string code;
    std::string field;
    std::string message;

    static Status Ok() { return {true, {}, {}, {}}; }
    static Status Fail(std::string code, std::string field, std::string message = {}) {
        return {false, std::move(code), std::move(field), std::move(message)};
    }
};

// Registry contract (NOT length conversion — no meter/feet):
//   sentiment priority: 긍정(1) → 부정(2) → 중립 Hub(3)
//   anchor keywords: "불만" ∈ 부정, "배송" ∈ category.main
constexpr const char* kHubSentiment = "중립";
constexpr const char* kAnchorText = "배송이 너무 늦어요. 불만입니다.";

}  // namespace entity
