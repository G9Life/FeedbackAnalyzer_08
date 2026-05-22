#pragma once

#include <vector>

#include "entity/Types.hpp"
#include "entity/UnitRegistry.hpp"

namespace entity {

class Converter {
public:
    explicit Converter(UnitRegistry registry);

    AnalysisResult classify(const Feedback& feedback) const;
    AggregateSnapshot aggregate(const std::vector<Feedback>& feedbacks) const;
    std::vector<Feedback> filter(const std::vector<Feedback>& feedbacks,
                                 const std::string& sentimentId,
                                 const std::string& categoryId) const;

    const UnitRegistry& registry() const { return registry_; }

private:
    UnitRegistry registry_;
};

}  // namespace entity
