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
    const entity::Converter conv = makeConverter();
    const entity::AggregateSnapshot subOnly =
        conv.aggregate({entity::Feedback("택배가 왔어요")});
    REQUIRE(subOnly.categoryCounts.at("배송") == 0);

    const entity::AggregateSnapshot anchor =
        conv.aggregate({entity::Feedback(entity::kAnchorText)});
    REQUIRE(anchor.categoryCounts.at("배송") >= 1);
}

TEST_CASE("test_filter_negative_shipping_returns_anchor_only", "[domain][red][TC-B-04]") {
    const entity::Converter conv = makeConverter();
    const std::vector<entity::Feedback> session = {
        entity::Feedback(entity::kAnchorText),
        entity::Feedback("감사합니다. 만족합니다"),
    };
    const std::vector<entity::Feedback> filtered = conv.filter(session, "부정", "배송");
    REQUIRE(filtered.size() == 1);
    REQUIRE(filtered[0].text == entity::kAnchorText);
    REQUIRE(conv.classify(filtered[0]).sentimentId == "부정");
}

TEST_CASE("test_classify_coexisting_keywords_returns_positive", "[domain][red][TC-B-05]") {
    const entity::Converter conv = makeConverter();
    const entity::AnalysisResult r =
        conv.classify(entity::Feedback("좋은데 아쉬워요 만족은 하는데 별로"));
    REQUIRE(r.sentimentId == "긍정");
}

TEST_CASE("test_classify_near_negative_purpose_line_returns_negative", "[domain][red][TC-B-06]") {
    const entity::Converter conv = makeConverter();
    const entity::AnalysisResult near =
        conv.classify(entity::Feedback("배송이 너무 늦어요. 화가 납니다."));
    REQUIRE(near.sentimentId == "부정");

    const entity::AnalysisResult anchor =
        conv.classify(entity::Feedback(entity::kAnchorText));
    REQUIRE(anchor.sentimentId == "부정");
}

TEST_CASE("test_aggregate_three_feedbacks_sum_equals_three", "[domain][red][TC-B-07]") {
    const entity::Converter conv = makeConverter();
    const entity::AggregateSnapshot snap = conv.aggregate(
        {entity::Feedback("최고입니다"), entity::Feedback("별로예요"),
         entity::Feedback("그냥 그래요")});
    const int sum = snap.sentimentCounts.at("긍정") + snap.sentimentCounts.at("부정") +
                    snap.sentimentCounts.at("중립");
    REQUIRE(sum == 3);
}

TEST_CASE("test_classify_neutral_text_returns_hub_neutral", "[domain][red][TC-B-07]") {
    const entity::Converter conv = makeConverter();
    const entity::AnalysisResult r =
        conv.classify(entity::Feedback("전반적으로 무난했습니다."));
    REQUIRE(r.sentimentId == entity::kHubSentiment);
}

TEST_CASE("test_registerUnit_sentiment_keyword_classifies_new_unit", "[domain][register][red]") {
    entity::UnitRegistry reg = entity::UnitRegistry::defaultSnapshot();
    const entity::Status st =
        reg.registerUnit("sentiment", "화남", {"짜증", "화남"});
    REQUIRE(st.ok);
    entity::Converter conv(reg);
    const entity::AnalysisResult r =
        conv.classify(entity::Feedback("화남:짜증납니다"));
    REQUIRE(r.sentimentId == "화남");
}

TEST_CASE("test_registerUnit_category_main_counts_after_register", "[domain][register][red]") {
    entity::UnitRegistry reg = entity::UnitRegistry::defaultSnapshot();
    const entity::Status st =
        reg.registerUnit("category", "해외배송", {"해외", "국제택배"}, {"해외배송"});
    REQUIRE(st.ok);
    entity::Converter conv(reg);
    const entity::AggregateSnapshot snap =
        conv.aggregate({entity::Feedback("해외배송이 늦습니다")});
    REQUIRE(snap.categoryCounts.at("해외배송") == 1);
}

TEST_CASE("test_registerUnit_duplicate_returns_duplicate_unit", "[domain][register][red]") {
    entity::UnitRegistry reg = entity::UnitRegistry::defaultSnapshot();
    REQUIRE(reg.registerUnit("sentiment", "화남", {"짜증"}).ok);
    const entity::Status dup = reg.registerUnit("sentiment", "화남", {"짜증"});
    REQUIRE_FALSE(dup.ok);
    REQUIRE(dup.code == "DUPLICATE_UNIT");
}

TEST_CASE("test_registerUnit_then_classify_anchor_still_negative", "[domain][register][red]") {
    entity::UnitRegistry reg = entity::UnitRegistry::defaultSnapshot();
    REQUIRE(reg.registerUnit("sentiment", "화남", {"짜증"}).ok);
    entity::Converter conv(reg);
    REQUIRE(conv.classify(entity::Feedback(entity::kAnchorText)).sentimentId == "부정");
}

TEST_CASE("test_registry_covers_all_default_categories", "[domain][coverage]") {
    entity::UnitRegistry reg = entity::UnitRegistry::defaultSnapshot();
    REQUIRE(reg.hasSentiment("긍정"));
    REQUIRE(reg.hasSentiment("부정"));
    REQUIRE(reg.hasCategory("품질"));
    REQUIRE(reg.hasCategory("가격"));
    REQUIRE(reg.hasCategory("서비스"));
    REQUIRE(reg.hasCategory("사용성"));
    entity::Converter conv(reg);
    const entity::AggregateSnapshot snap = conv.aggregate(
        {entity::Feedback("품질이 나빠요"), entity::Feedback("가격이 비싸요"),
         entity::Feedback("서비스가 친절해요"), entity::Feedback("사용이 어려워요")});
    REQUIRE(snap.categoryCounts.at("품질") >= 1);
    REQUIRE(snap.categoryCounts.at("가격") >= 1);
    REQUIRE(snap.categoryCounts.at("서비스") >= 1);
    REQUIRE(snap.categoryCounts.at("사용성") >= 1);
    REQUIRE(reg.replaceAll(reg).ok);
    const std::vector<entity::Feedback> allSentiment =
        conv.filter({entity::Feedback(entity::kAnchorText), entity::Feedback("친절합니다")},
                    "전체", "전체");
    REQUIRE(allSentiment.size() == 2);
}

TEST_CASE("test_registerUnit_reflects_in_filter_same_registry", "[domain][register][red]") {
    entity::UnitRegistry reg = entity::UnitRegistry::defaultSnapshot();
    REQUIRE(reg.registerUnit("category", "해외배송", {"해외"}, {"해외배송"}).ok);
    entity::Converter conv(reg);
    const std::string overseas = "해외배송이 늦어 불만입니다";
    const std::vector<entity::Feedback> session = {
        entity::Feedback(entity::kAnchorText),
        entity::Feedback(overseas),
    };
    const std::vector<entity::Feedback> filtered = conv.filter(session, "부정", "해외배송");
    REQUIRE(filtered.size() == 1);
    REQUIRE(filtered[0].text == overseas);
}
