#pragma once

#include <string>
#include <vector>

#include "entity/Types.hpp"

namespace entity {

class UnitRegistry {
public:
    static UnitRegistry defaultSnapshot();

    Status registerUnit(const std::string& type,
                        const std::string& unitId,
                        const std::vector<std::string>& keywords,
                        const std::vector<std::string>& mainKeywords = {});

    Status replaceAll(const UnitRegistry& snapshot);

    bool hasSentiment(const std::string& id) const;
    bool hasCategory(const std::string& id) const;
    std::size_t sentimentUnitCount() const;
    std::size_t categoryUnitCount() const;

private:
    std::size_t sentimentCount_{0};
    std::size_t categoryCount_{0};
};

}  // namespace entity
