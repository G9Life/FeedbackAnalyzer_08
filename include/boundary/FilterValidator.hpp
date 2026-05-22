#pragma once

#include <string>

#include "entity/Types.hpp"
#include "entity/UnitRegistry.hpp"

namespace boundary {

class FilterValidator {
public:
    explicit FilterValidator(entity::UnitRegistry registry);

    entity::Status validate(const std::string& sentimentFilter,
                            const std::string& categoryFilter) const;

private:
    entity::UnitRegistry registry_;
};

}  // namespace boundary
