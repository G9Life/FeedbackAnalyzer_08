// Boundary layer RED skeleton — 본문 구현 금지, FAIL("RED") only.
//
// Target files / classes:
//   include/boundary/FeedbackParser.hpp   — class FeedbackParser
//   include/boundary/FilterValidator.hpp  — class FilterValidator
//
// Future / legacy HTTP (미구현):
//   src/cpp/main.cpp — POST /analyze, /upload, /filter, GET /download

#include <catch2/catch_test_macros.hpp>

// =============================================================================
// Track A — UI / Boundary (README TC-A-01 ~ TC-A-07)
// =============================================================================

TEST_CASE("test_parse_plain_empty_text_returns_empty_text", "[boundary][parse][red][TC-A-01]") {
    // Target: FeedbackParser::parsePlainText — EMPTY_TEXT
    FAIL("RED");
}

TEST_CASE("test_parse_labelbody_colon_missing_throws_colon_missing", "[boundary][parse][red][TC-A-02]") {
    // Target: FeedbackParser::parseLabelBody — COLON_MISSING
    FAIL("RED");
}

TEST_CASE("test_parse_labelbody_unknown_emotion_rejects_hub_substitute", "[boundary][parse][red][TC-A-03]") {
    // Target: FeedbackParser::parseLabelBody — UNKNOWN_EMOTION
    FAIL("RED");
}

TEST_CASE("test_upload_csv_missing_text_column_rejects", "[boundary][http][red][TC-A-04]") {
    // Target: main.cpp POST /upload — CSV_MISSING_TEXT_COLUMN
    FAIL("RED");
}

TEST_CASE("test_analyze_trim_anchor_stored_text", "[boundary][http][red][TC-A-05]") {
    // Target: main.cpp POST /analyze — trim stored body
    FAIL("RED");
}

TEST_CASE("test_filter_negative_shipping_session_one_item", "[boundary][http][red][TC-A-06]") {
    // Target: main.cpp POST /filter — sentiment=부정, keyword=배송
    FAIL("RED");
}

TEST_CASE("test_download_csv_anchor_one_data_row", "[boundary][http][red][TC-A-07]") {
    // Target: main.cpp GET /download — text header + 1 data row
    FAIL("RED");
}

TEST_CASE("test_download_empty_fil_data_returns_not_found", "[boundary][http][red][TC-A-07]") {
    // Target: main.cpp GET /download — NO_DATA_TO_EXPORT 404
    FAIL("RED");
}

// =============================================================================
// 파싱 형식 오류 (보조)
// =============================================================================

TEST_CASE("test_parse_labelbody_empty_label_returns_empty_label", "[boundary][parse][red]") {
    // Target: FeedbackParser::parseLabelBody — EMPTY_LABEL
    FAIL("RED");
}

TEST_CASE("test_parse_labelbody_empty_body_returns_empty_body", "[boundary][parse][red]") {
    // Target: FeedbackParser::parseLabelBody — EMPTY_BODY
    FAIL("RED");
}

// =============================================================================
// 필터 불가 문자열 (FilterValidator)
// =============================================================================

TEST_CASE("test_filter_unknown_category_rejects_overseas_keyword", "[boundary][filter][red]") {
    // Target: FilterValidator::validate — UNKNOWN_CATEGORY_FILTER
    FAIL("RED");
}

TEST_CASE("test_filter_unknown_sentiment_rejects_angry_label", "[boundary][filter][red]") {
    // Target: FilterValidator::validate — UNKNOWN_SENTIMENT_FILTER
    FAIL("RED");
}

TEST_CASE("test_filter_empty_sentiment_token_rejects", "[boundary][filter][red]") {
    // Target: FilterValidator::validate
    FAIL("RED");
}

TEST_CASE("test_filter_garbage_category_string_rejects", "[boundary][filter][red]") {
    // Target: FilterValidator::validate
    FAIL("RED");
}

TEST_CASE("test_filter_numeric_sentiment_string_rejects", "[boundary][filter][red]") {
    // Target: FilterValidator::validate
    FAIL("RED");
}
