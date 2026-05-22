// Data layer RED skeleton — 본문 구현 금지, FAIL("RED") only.
//
// Target files / classes:
//   include/data/ConfigLoader.hpp — class ConfigLoader
//   tests/fixtures/registry_default.yaml
//   tests/fixtures/registry_valid.json
//   tests/fixtures/registry_invalid_hub.yaml
//   tests/fixtures/registry_missing_version.json

#include <catch2/catch_test_macros.hpp>

// =============================================================================
// JSON — ConfigLoader::loadJson / loadRatios
// =============================================================================

TEST_CASE("test_config_json_valid_loads_registry_snapshot", "[data][json][red]") {
    // Target: ConfigLoader::loadJson
    FAIL("RED");
}

TEST_CASE("test_config_json_valid_classify_anchor_negative", "[data][json][red]") {
    // Target: ConfigLoader::loadJson + Converter::classify
    FAIL("RED");
}

TEST_CASE("test_config_json_missing_file_fails_config_parse", "[data][json][red]") {
    // Target: ConfigLoader::loadJson — CONFIG_PARSE_ERROR
    FAIL("RED");
}

TEST_CASE("test_config_json_missing_version_fails_unsupported", "[data][json][red]") {
    // Target: ConfigLoader::loadJson — UNSUPPORTED_CONFIG_VERSION
    FAIL("RED");
}

TEST_CASE("test_config_json_load_then_aggregate_anchor_shipping", "[data][json][red]") {
    // Target: ConfigLoader::loadJson + Converter::aggregate
    FAIL("RED");
}

// =============================================================================
// YAML — ConfigLoader::loadYaml / loadRatios
// =============================================================================

TEST_CASE("test_config_yaml_valid_loads_default_ratios", "[data][yaml][red]") {
    // Target: ConfigLoader::loadYaml
    FAIL("RED");
}

TEST_CASE("test_config_yaml_valid_anchor_classify_negative", "[data][yaml][red]") {
    // Target: ConfigLoader::loadYaml + Converter::classify
    FAIL("RED");
}

TEST_CASE("test_config_yaml_invalid_hub_fails_invalid_hub_config", "[data][yaml][red]") {
    // Target: ConfigLoader::loadYaml — INVALID_HUB_CONFIG
    FAIL("RED");
}

TEST_CASE("test_config_yaml_missing_file_fails_parse_error", "[data][yaml][red]") {
    // Target: ConfigLoader::loadYaml — CONFIG_PARSE_ERROR
    FAIL("RED");
}

TEST_CASE("test_config_yaml_load_ratios_enum_yaml_format", "[data][yaml][red]") {
    // Target: ConfigLoader::loadRatios(ConfigFormat::Yaml)
    FAIL("RED");
}
