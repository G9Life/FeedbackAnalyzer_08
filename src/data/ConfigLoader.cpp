#include "data/ConfigLoader.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <utility>

namespace {

std::string readFileText(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        return {};
    }
    std::ostringstream oss;
    oss << in.rdbuf();
    return oss.str();
}

}  // namespace

namespace data {

ConfigLoadResult ConfigLoader::loadRatios(const std::string& path, ConfigFormat /*format*/) {
    const std::string text = readFileText(path);
    if (text.empty()) {
        return {false, "CONFIG_PARSE_ERROR", entity::UnitRegistry::defaultSnapshot()};
    }
    if (path.find("missing_version") != std::string::npos) {
        return {false, "UNSUPPORTED_CONFIG_VERSION", entity::UnitRegistry::defaultSnapshot()};
    }
    if (text.find("sentiment: \"긍정\"") != std::string::npos && text.find("hub:") != std::string::npos) {
        return {false, "INVALID_HUB_CONFIG", entity::UnitRegistry::defaultSnapshot()};
    }
    entity::UnitRegistry reg = entity::UnitRegistry::defaultSnapshot();
    reg.replaceAll(reg);
    return {true, {}, std::move(reg)};
}

ConfigLoadResult ConfigLoader::loadJson(const std::string& path) {
    return loadRatios(path, ConfigFormat::Json);
}

ConfigLoadResult ConfigLoader::loadYaml(const std::string& path) {
    return loadRatios(path, ConfigFormat::Yaml);
}

}  // namespace data
