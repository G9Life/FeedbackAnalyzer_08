cmake_minimum_required(VERSION 3.14)
project(FeedbackAnalyzer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(FEEDBACK_ANALYZER_RED_PHASE "TDD RED: stub core (tests fail until GREEN src/)" OFF)
option(ENABLE_COVERAGE "gcov instrumentation for entity/boundary coverage" OFF)

if(ENABLE_COVERAGE)
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

add_executable(feedback_analyzer
    src/cpp/main.cpp
    src/cpp/Constants.cpp
    src/cpp/Filters.cpp
    src/cpp/Logger.cpp
    src/cpp/Session.cpp
    src/cpp/TextAnalyzer.cpp
    src/cpp/UIComponents.cpp
)

target_include_directories(feedback_analyzer PRIVATE src/cpp)

if(WIN32)
    target_compile_definitions(feedback_analyzer PRIVATE
        _WIN32_WINNT=0x0A00
        WINVER=0x0A00
    )
    target_link_libraries(feedback_analyzer PRIVATE ws2_32)
endif()

include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.4
)
FetchContent_MakeAvailable(Catch2)

if(FEEDBACK_ANALYZER_RED_PHASE)
    add_library(feedback_analyzer_core tests/red_phase_stubs.cpp)
else()
    add_library(feedback_analyzer_core
        src/entity/EntityCore.cpp
        src/boundary/BoundaryCore.cpp
        src/data/ConfigLoader.cpp
    )
endif()

target_include_directories(feedback_analyzer_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(feedback_analyzer PRIVATE feedback_analyzer_core)
target_include_directories(feedback_analyzer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

enable_testing()

add_executable(feedback_analyzer_tests
    tests/domain_tests.cpp
    tests/boundary_tests.cpp
    tests/data_tests.cpp
)

target_compile_definitions(feedback_analyzer_tests
    PRIVATE
        TEST_FIXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures"
)

target_link_libraries(feedback_analyzer_tests
    PRIVATE
        feedback_analyzer_core
        Catch2::Catch2WithMain
)

include(CTest)
include(Catch)
catch_discover_tests(feedback_analyzer_tests)
