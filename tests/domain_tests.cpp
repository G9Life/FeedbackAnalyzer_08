// Domain layer — TC-B-01, TC-B-02 GREEN; 나머지 RED.

#include <catch2/catch_test_macros.hpp>

#include "entity/Converter.hpp"
#include "entity/Types.hpp"
#include "entity/UnitRegistry.hpp"

namespace {

entity::Converter makeConverter() {
    return entity::Converter(entity::UnitRegistry::defaultSnapshot());
}

}  // namespace

TEST_CASE("test_classify_anchor_text_returns_negative", "[domain][red][TC-B-01]") {
    const entity::Converter conv = makeConverter();
    const entity::AnalysisResult r = conv.classify(entity::Feedback(entity::kAnchorText));
    REQUIRE(r.sentimentId == "부정");
}

TEST_CASE("test_aggregate_anchor_single_negative_and_shipping_one", "[domain][red][TC-B-02]") {
    const entity::Converter conv = makeConverter();
    const entity::AggregateSnapshot snap =
        conv.aggregate({entity::Feedback(entity::kAnchorText)});
    REQUIRE(snap.sentimentCounts.at("부정") == 1);
    REQUIRE(snap.categoryCounts.at("배송") == 1);
    REQUIRE(snap.sentimentCounts.at("긍정") == 0);
    REQUIRE(snap.sentimentCounts.at("중립") == 0);
}

TEST_CASE("test_aggregate_subkeyword_only_shipping_zero", "[domain][red][TC-B-03]") {
    FAIL("RED");
}

TEST_CASE("test_filter_negative_shipping_returns_anchor_only", "[domain][red][TC-B-04]") {
    FAIL("RED");
}

TEST_CASE("test_classify_coexisting_keywords_returns_positive", "[domain][red][TC-B-05]") {
    FAIL("RED");
}

TEST_CASE("test_classify_near_negative_purpose_line_returns_negative", "[domain][red][TC-B-06]") {
    FAIL("RED");
}

TEST_CASE("test_aggregate_three_feedbacks_sum_equals_three", "[domain][red][TC-B-07]") {
    FAIL("RED");
}

TEST_CASE("test_classify_neutral_text_returns_hub_neutral", "[domain][red][TC-B-07]") {
    FAIL("RED");
}

TEST_CASE("test_registerUnit_sentiment_keyword_classifies_new_unit", "[domain][register][red]") {
    FAIL("RED");
}

TEST_CASE("test_registerUnit_category_main_counts_after_register", "[domain][register][red]") {
    FAIL("RED");
}

TEST_CASE("test_registerUnit_duplicate_returns_duplicate_unit", "[domain][register][red]") {
    FAIL("RED");
}

TEST_CASE("test_registerUnit_then_classify_anchor_still_negative", "[domain][register][red]") {
    FAIL("RED");
}

TEST_CASE("test_registerUnit_reflects_in_filter_same_registry", "[domain][register][red]") {
    FAIL("RED");
}
