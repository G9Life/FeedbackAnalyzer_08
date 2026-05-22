#include <catch2/catch_test_macros.hpp>

#include <sstream>
#include <string>
#include <vector>

#include "boundary/CsvPresenter.hpp"
#include "boundary/CsvUploadValidator.hpp"
#include "boundary/FeedbackParser.hpp"
#include "boundary/FilterValidator.hpp"
#include "entity/Converter.hpp"
#include "entity/Types.hpp"
#include "entity/UnitRegistry.hpp"

TEST_CASE("test_parse_plain_empty_text_returns_empty_text", "[boundary][parse][red][TC-A-01]") {
    const boundary::ParseResult empty = boundary::FeedbackParser::parsePlainText("");
    REQUIRE_FALSE(empty.success);
    REQUIRE(empty.error.code == "EMPTY_TEXT");
    REQUIRE(empty.error.field == "text");

    const boundary::ParseResult ws = boundary::FeedbackParser::parsePlainText("\t\n");
    REQUIRE_FALSE(ws.success);
    REQUIRE(ws.error.code == "EMPTY_TEXT");
}

TEST_CASE("test_parse_labelbody_colon_missing_throws_colon_missing", "[boundary][parse][red][TC-A-02]") {
    const boundary::ParseResult r = boundary::FeedbackParser::parseLabelBody("만족합니다");
    REQUIRE_FALSE(r.success);
    REQUIRE(r.error.code == "COLON_MISSING");
    REQUIRE(r.error.field == "text");
}

TEST_CASE("test_parse_labelbody_unknown_emotion_rejects_hub_substitute",
          "[boundary][parse][red][TC-A-03]") {
    const boundary::ParseResult r = boundary::FeedbackParser::parseLabelBody("화남:짜증납니다");
    REQUIRE_FALSE(r.success);
    REQUIRE(r.error.code == "UNKNOWN_EMOTION");
    REQUIRE(r.error.message == "화남");
    REQUIRE(r.label.empty());
}

TEST_CASE("test_upload_csv_missing_text_column_rejects", "[boundary][http][red][TC-A-04]") {
    const boundary::CsvValidationResult r =
        boundary::CsvUploadValidator::validate("foo,bar\na,b");
    REQUIRE_FALSE(r.success);
    REQUIRE(r.error.code == "CSV_MISSING_TEXT_COLUMN");
    REQUIRE(r.error.field == "file");
}

TEST_CASE("test_analyze_trim_anchor_stored_text", "[boundary][http][red][TC-A-05]") {
    const boundary::ParseResult r = boundary::FeedbackParser::parsePlainText(
        "  배송이 너무 늦어요. 불만입니다.  ");
    REQUIRE(r.success);
    REQUIRE(r.body == entity::kAnchorText);
}

TEST_CASE("test_filter_negative_shipping_session_one_item", "[boundary][http][red][TC-A-06]") {
    const entity::Converter conv(entity::UnitRegistry::defaultSnapshot());
    const std::vector<entity::Feedback> session = {
        entity::Feedback(entity::kAnchorText),
        entity::Feedback("친절합니다"),
    };
    const std::vector<entity::Feedback> filtered = conv.filter(session, "부정", "배송");
    REQUIRE(filtered.size() == 1);
    REQUIRE(filtered[0].text == entity::kAnchorText);
    REQUIRE(conv.classify(filtered[0]).sentimentId == "부정");
}

TEST_CASE("test_download_csv_anchor_one_data_row", "[boundary][http][red][TC-A-07]") {
    const boundary::CsvRenderResult r =
        boundary::CsvPresenter::render({entity::Feedback(entity::kAnchorText)});
    REQUIRE(r.success);
    REQUIRE(r.httpStatus == 200);
    REQUIRE(r.body.find("text") == 0);
    REQUIRE(r.body.find("불만") != std::string::npos);
    REQUIRE(r.body.find("배송") != std::string::npos);

    std::vector<std::string> lines;
    std::istringstream iss(r.body);
    std::string line;
    while (std::getline(iss, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        lines.push_back(line);
    }
    REQUIRE(lines.size() == 2);
}

TEST_CASE("test_download_empty_fil_data_returns_not_found", "[boundary][http][red][TC-A-07]") {
    const boundary::CsvRenderResult r = boundary::CsvPresenter::render({});
    REQUIRE_FALSE(r.success);
    REQUIRE(r.error.code == "NO_DATA_TO_EXPORT");
    REQUIRE(r.httpStatus == 404);
}

TEST_CASE("test_parse_labelbody_empty_label_returns_empty_label", "[boundary][parse][red]") {
    const boundary::ParseResult r = boundary::FeedbackParser::parseLabelBody(":본문만");
    REQUIRE_FALSE(r.success);
    REQUIRE(r.error.code == "EMPTY_LABEL");
}

TEST_CASE("test_parse_labelbody_empty_body_returns_empty_body", "[boundary][parse][red]") {
    const boundary::ParseResult r = boundary::FeedbackParser::parseLabelBody("부정:");
    REQUIRE_FALSE(r.success);
    REQUIRE(r.error.code == "EMPTY_BODY");
}

TEST_CASE("test_filter_unknown_category_rejects_overseas_keyword", "[boundary][filter][red]") {
    const boundary::FilterValidator validator(entity::UnitRegistry::defaultSnapshot());
    const entity::Status st = validator.validate("부정", "해외배송");
    REQUIRE_FALSE(st.ok);
    REQUIRE(st.code == "UNKNOWN_CATEGORY_FILTER");
}

TEST_CASE("test_filter_unknown_sentiment_rejects_angry_label", "[boundary][filter][red]") {
    const boundary::FilterValidator validator(entity::UnitRegistry::defaultSnapshot());
    const entity::Status st = validator.validate("화남", "전체");
    REQUIRE_FALSE(st.ok);
    REQUIRE(st.code == "UNKNOWN_SENTIMENT_FILTER");
}

TEST_CASE("test_filter_empty_sentiment_token_rejects", "[boundary][filter][red]") {
    const boundary::FilterValidator validator(entity::UnitRegistry::defaultSnapshot());
    const entity::Status st = validator.validate("   ", "전체");
    REQUIRE_FALSE(st.ok);
    REQUIRE(st.code == "UNKNOWN_SENTIMENT_FILTER");
}

TEST_CASE("test_filter_garbage_category_string_rejects", "[boundary][filter][red]") {
    const boundary::FilterValidator validator(entity::UnitRegistry::defaultSnapshot());
    const entity::Status st = validator.validate("부정", "not_a_category");
    REQUIRE_FALSE(st.ok);
    REQUIRE(st.code == "UNKNOWN_CATEGORY_FILTER");
}

TEST_CASE("test_filter_numeric_sentiment_string_rejects", "[boundary][filter][red]") {
    const boundary::FilterValidator validator(entity::UnitRegistry::defaultSnapshot());
    const entity::Status st = validator.validate("123", "전체");
    REQUIRE_FALSE(st.ok);
    REQUIRE(st.code == "UNKNOWN_SENTIMENT_FILTER");
}
