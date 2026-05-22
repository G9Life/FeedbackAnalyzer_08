#pragma once

#include <string>

#include "entity/UnitRegistry.hpp"

namespace data {

enum class ConfigFormat { Json, Yaml };

struct ConfigLoadResult {
    bool success{false};
    std::string errorCode;
    entity::UnitRegistry registry;
};

class ConfigLoader {
public:
    static ConfigLoadResult loadRatios(const std::string& path, ConfigFormat format);
    static ConfigLoadResult loadJson(const std::string& path);
    static ConfigLoadResult loadYaml(const std::string& path);
};

}  // namespace data
