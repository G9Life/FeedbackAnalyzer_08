// Domain layer RED skeleton — 본문 구현 금지, FAIL("RED") only.
//
// Target files / classes:
//   include/entity/Converter.hpp    — class Converter
//   include/entity/UnitRegistry.hpp — class UnitRegistry
//   include/entity/Types.hpp        — struct Feedback, AnalysisResult, AggregateSnapshot
//
// Legacy reference (GREEN 전 이전):
//   src/cpp/TextAnalyzer.h          — class TextAnalyzer
//   src/cpp/Constants.cpp           — Constants::SENTIMENT_KEYWORDS, CATEGORY_KEYWORDS

#include <catch2/catch_test_macros.hpp>

// =============================================================================
// Track B — Domain / Logic (README TC-B-01 ~ TC-B-07)
// =============================================================================

TEST_CASE("test_classify_anchor_text_returns_negative", "[domain][red][TC-B-01]") {
    // Target: Converter::classify
    FAIL("RED");
}

TEST_CASE("test_aggregate_anchor_single_negative_and_shipping_one", "[domain][red][TC-B-02]") {
    // Target: Converter::aggregate
    FAIL("RED");
}

TEST_CASE("test_aggregate_subkeyword_only_shipping_zero", "[domain][red][TC-B-03]") {
    // Target: Converter::aggregate — main-only (C-D06)
    FAIL("RED");
}

TEST_CASE("test_filter_negative_shipping_returns_anchor_only", "[domain][red][TC-B-04]") {
    // Target: Converter::filter
    FAIL("RED");
}

TEST_CASE("test_classify_coexisting_keywords_returns_positive", "[domain][red][TC-B-05]") {
    // Target: Converter::classify — priority 긍정→부정→중립
    FAIL("RED");
}

TEST_CASE("test_classify_near_negative_purpose_line_returns_negative", "[domain][red][TC-B-06]") {
    // Target: Converter::classify — "배송이 너무 늦어요. 화가 납니다."
    FAIL("RED");
}

TEST_CASE("test_aggregate_three_feedbacks_sum_equals_three", "[domain][red][TC-B-07]") {
    // Target: Converter::aggregate — 건수 합 보존 (INV-D5)
    FAIL("RED");
}

TEST_CASE("test_classify_neutral_text_returns_hub_neutral", "[domain][red][TC-B-07]") {
    // Target: Converter::classify — Hub "중립"
    FAIL("RED");
}

// =============================================================================
// 동적 등록 — UnitRegistry::registerUnit
// =============================================================================

TEST_CASE("test_registerUnit_sentiment_keyword_classifies_new_unit", "[domain][register][red]") {
    // Target: UnitRegistry::registerUnit — type=sentiment
    FAIL("RED");
}

TEST_CASE("test_registerUnit_category_main_counts_after_register", "[domain][register][red]") {
    // Target: UnitRegistry::registerUnit — type=category, main keywords
    FAIL("RED");
}

TEST_CASE("test_registerUnit_duplicate_returns_duplicate_unit", "[domain][register][red]") {
    // Target: UnitRegistry::registerUnit — DUPLICATE_UNIT
    FAIL("RED");
}

TEST_CASE("test_registerUnit_then_classify_anchor_still_negative", "[domain][register][red]") {
    // Target: UnitRegistry + Converter::classify
    FAIL("RED");
}

TEST_CASE("test_registerUnit_reflects_in_filter_same_registry", "[domain][register][red]") {
    // Target: UnitRegistry + Converter::filter — INV-D1
    FAIL("RED");
}
