#include "entity/Converter.hpp"
#include "entity/UnitRegistry.hpp"

#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace {

using KeywordList = std::vector<std::string>;

struct RegistryState {
    KeywordList positive;
    KeywordList negative;
    std::map<std::string, KeywordList> dynamicSentiment;
    std::map<std::string, KeywordList> categoryMain;
    std::set<std::string> sentimentIds;
    std::set<std::string> categoryIds;
    std::set<std::string> dynamicKeys;
};

RegistryState& state() {
    static RegistryState s;
    return s;
}

void initDefaultKeywords() {
    RegistryState& s = state();
    s.positive = {"좋아요", "만족", "감사", "최고", "좋은", "훌륭", "추천", "좋았어요",
                  "좋습니다", "최고입니다", "감사합니다", "만족스럽", "좋았습니다",
                  "최고에요", "기뻐요", "만족합니다", "굿", "최고다", "와우", "아주 좋아"};
    s.negative = {"나쁘", "불만", "실망", "최악", "별로", "불편", "불만족", "문제",
                  "불량", "불량품", "환불", "교환", "불만족스럽", "실망스럽",
                  "짜증", "화남", "화가", "별로에요", "엉망", "최악이다", "실패", "구려"};
    s.categoryMain.clear();
    s.categoryMain["배송"] = {"배송", "배달", "물류", "배송지연", "배송시간", "퀵", "소포"};
    s.categoryMain["품질"] = {"품질", "재질", "내구성", "마감", "제품상태", "품질문제", "내용물", "고장"};
    s.categoryMain["가격"] = {"가격", "비용", "할인", "가성비", "가격대", "비싸", "저렴", "금액", "요금"};
    s.categoryMain["서비스"] = {"서비스", "응대", "상담", "문의", "답변", "고객서비스", "친절", "불친절"};
    s.categoryMain["사용성"] = {"사용", "편리", "불편", "사용법", "설명서", "사용방법", "어렵", "쉽게"};
    s.sentimentIds = {"긍정", "부정", "중립"};
    s.categoryIds = {"배송", "품질", "가격", "서비스", "사용성"};
    s.dynamicSentiment.clear();
    s.dynamicKeys.clear();
}

bool containsAny(const std::string& text, const KeywordList& keywords) {
    for (const auto& kw : keywords) {
        if (!kw.empty() && text.find(kw) != std::string::npos) {
            return true;
        }
    }
    return false;
}

std::string classifyText(const std::string& text) {
    const RegistryState& s = state();
    for (const auto& entry : s.dynamicSentiment) {
        if (containsAny(text, entry.second)) {
            return entry.first;
        }
    }
    if (containsAny(text, s.positive)) {
        return "긍정";
    }
    if (containsAny(text, s.negative)) {
        return "부정";
    }
    return "중립";
}

std::vector<std::string> categoryHits(const std::string& text) {
    std::vector<std::string> hits;
    for (const auto& entry : state().categoryMain) {
        if (containsAny(text, entry.second)) {
            hits.push_back(entry.first);
        }
    }
    return hits;
}

}  // namespace

namespace entity {

UnitRegistry UnitRegistry::defaultSnapshot() {
    initDefaultKeywords();
    UnitRegistry reg;
    reg.sentimentCount_ = state().sentimentIds.size();
    reg.categoryCount_ = state().categoryIds.size();
    return reg;
}

Status UnitRegistry::registerUnit(const std::string& type,
                                  const std::string& unitId,
                                  const std::vector<std::string>& keywords,
                                  const std::vector<std::string>& mainKeywords) {
    const std::string key = type + ":" + unitId;
    if (state().dynamicKeys.count(key)) {
        return Status::Fail("DUPLICATE_UNIT", "");
    }
    state().dynamicKeys.insert(key);

    if (type == "sentiment") {
        state().sentimentIds.insert(unitId);
        state().dynamicSentiment[unitId] = keywords;
        sentimentCount_ = state().sentimentIds.size();
    } else if (type == "category") {
        state().categoryIds.insert(unitId);
        state().categoryMain[unitId] = mainKeywords.empty() ? keywords : mainKeywords;
        categoryCount_ = state().categoryIds.size();
    }
    return Status::Ok();
}

Status UnitRegistry::replaceAll(const UnitRegistry& snapshot) {
    initDefaultKeywords();
    sentimentCount_ = snapshot.sentimentCount_;
    categoryCount_ = snapshot.categoryCount_;
    return Status::Ok();
}

bool UnitRegistry::hasSentiment(const std::string& id) const {
    return state().sentimentIds.count(id) > 0;
}

bool UnitRegistry::hasCategory(const std::string& id) const {
    return state().categoryIds.count(id) > 0;
}

std::size_t UnitRegistry::sentimentUnitCount() const {
    return sentimentCount_ > 0 ? sentimentCount_ : state().sentimentIds.size();
}

std::size_t UnitRegistry::categoryUnitCount() const {
    return categoryCount_ > 0 ? categoryCount_ : state().categoryIds.size();
}

Converter::Converter(UnitRegistry registry) : registry_(std::move(registry)) {}

AnalysisResult Converter::classify(const Feedback& feedback) const {
    AnalysisResult result;
    result.sentimentId = classifyText(feedback.text);
    result.categoryIds = categoryHits(feedback.text);
    return result;
}

AggregateSnapshot Converter::aggregate(const std::vector<Feedback>& feedbacks) const {
    AggregateSnapshot snap;
    for (const auto& id : state().sentimentIds) {
        snap.sentimentCounts[id] = 0;
    }
    for (const auto& entry : state().categoryMain) {
        snap.categoryCounts[entry.first] = 0;
    }
    for (const auto& fb : feedbacks) {
        snap.sentimentCounts[classifyText(fb.text)]++;
        for (const auto& cat : categoryHits(fb.text)) {
            snap.categoryCounts[cat]++;
        }
    }
    return snap;
}

std::vector<Feedback> Converter::filter(const std::vector<Feedback>& feedbacks,
                                        const std::string& sentimentId,
                                        const std::string& categoryId) const {
    std::vector<Feedback> out;
    for (const auto& fb : feedbacks) {
        const AnalysisResult r = classify(fb);
        if (sentimentId != "전체" && r.sentimentId != sentimentId) {
            continue;
        }
        if (categoryId != "전체") {
            if (std::find(r.categoryIds.begin(), r.categoryIds.end(), categoryId) ==
                r.categoryIds.end()) {
                continue;
            }
        }
        out.push_back(fb);
    }
    return out;
}

}  // namespace entity
