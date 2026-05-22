#include <catch2/catch_test_macros.hpp>

#include <string>

#include "data/ConfigLoader.hpp"
#include "entity/Converter.hpp"
#include "entity/Types.hpp"

namespace {

std::string fixturePath(const char* name) {
    return std::string(TEST_FIXTURE_DIR) + "/" + name;
}

}  // namespace

TEST_CASE("test_config_json_valid_loads_registry_snapshot", "[data][json][red]") {
    const data::ConfigLoadResult r =
        data::ConfigLoader::loadJson(fixturePath("registry_valid.json"));
    REQUIRE(r.success);
    REQUIRE(r.registry.sentimentUnitCount() >= 3);
}

TEST_CASE("test_config_json_valid_classify_anchor_negative", "[data][json][red]") {
    const data::ConfigLoadResult r =
        data::ConfigLoader::loadJson(fixturePath("registry_valid.json"));
    REQUIRE(r.success);
    entity::Converter conv(r.registry);
    REQUIRE(conv.classify(entity::Feedback(entity::kAnchorText)).sentimentId == "부정");
}

TEST_CASE("test_config_json_missing_file_fails_config_parse", "[data][json][red]") {
    const data::ConfigLoadResult r =
        data::ConfigLoader::loadJson(fixturePath("no_such_registry.json"));
    REQUIRE_FALSE(r.success);
    REQUIRE(r.errorCode == "CONFIG_PARSE_ERROR");
}

TEST_CASE("test_config_json_missing_version_fails_unsupported", "[data][json][red]") {
    const data::ConfigLoadResult r =
        data::ConfigLoader::loadJson(fixturePath("registry_missing_version.json"));
    REQUIRE_FALSE(r.success);
    REQUIRE(r.errorCode == "UNSUPPORTED_CONFIG_VERSION");
}

TEST_CASE("test_config_json_load_then_aggregate_anchor_shipping", "[data][json][red]") {
    const data::ConfigLoadResult r =
        data::ConfigLoader::loadJson(fixturePath("registry_valid.json"));
    REQUIRE(r.success);
    entity::Converter conv(r.registry);
    const entity::AggregateSnapshot snap =
        conv.aggregate({entity::Feedback(entity::kAnchorText)});
    REQUIRE(snap.sentimentCounts.at("부정") == 1);
    REQUIRE(snap.categoryCounts.at("배송") == 1);
}

TEST_CASE("test_config_yaml_valid_loads_default_ratios", "[data][yaml][red]") {
    const data::ConfigLoadResult r =
        data::ConfigLoader::loadYaml(fixturePath("registry_default.yaml"));
    REQUIRE(r.success);
    REQUIRE(r.registry.categoryUnitCount() >= 1);
}

TEST_CASE("test_config_yaml_valid_anchor_classify_negative", "[data][yaml][red]") {
    const data::ConfigLoadResult r =
        data::ConfigLoader::loadYaml(fixturePath("registry_default.yaml"));
    REQUIRE(r.success);
    entity::Converter conv(r.registry);
    REQUIRE(conv.classify(entity::Feedback(entity::kAnchorText)).sentimentId == "부정");
}

TEST_CASE("test_config_yaml_invalid_hub_fails_invalid_hub_config", "[data][yaml][red]") {
    const data::ConfigLoadResult r =
        data::ConfigLoader::loadYaml(fixturePath("registry_invalid_hub.yaml"));
    REQUIRE_FALSE(r.success);
    REQUIRE(r.errorCode == "INVALID_HUB_CONFIG");
}

TEST_CASE("test_config_yaml_missing_file_fails_parse_error", "[data][yaml][red]") {
    const data::ConfigLoadResult r =
        data::ConfigLoader::loadYaml(fixturePath("missing_registry.yaml"));
    REQUIRE_FALSE(r.success);
    REQUIRE(r.errorCode == "CONFIG_PARSE_ERROR");
}

TEST_CASE("test_config_yaml_load_ratios_enum_yaml_format", "[data][yaml][red]") {
    const data::ConfigLoadResult r = data::ConfigLoader::loadRatios(
        fixturePath("registry_default.yaml"), data::ConfigFormat::Yaml);
    REQUIRE(r.success);
    entity::Converter conv(r.registry);
    REQUIRE(conv.classify(entity::Feedback(entity::kAnchorText)).sentimentId == "부정");
}
