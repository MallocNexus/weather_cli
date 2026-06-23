#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "service/weather_service.hpp"
#include <nlohmann/json.hpp>
#include <stdexcept>

// ---------------------------------------------------------------------------
// Helpers — build minimal valid Open-Meteo JSON responses as strings.
// ---------------------------------------------------------------------------

static std::string ValidJson(
    double temp       = 18.5,
    double feels_like = 16.2,
    int    humidity   = 85,
    double wind       = 26.1,
    int    wmo        = 61,
    double daily_max  = 21.0,
    double daily_min  = 12.5) {
    return R"({
        "current": {
            "temperature_2m":        )" + std::to_string(temp)       + R"(,
            "apparent_temperature":  )" + std::to_string(feels_like) + R"(,
            "relative_humidity_2m":  )" + std::to_string(humidity)   + R"(,
            "wind_speed_10m":        )" + std::to_string(wind)       + R"(,
            "weather_code":          )" + std::to_string(wmo)        + R"(
        },
        "daily": {
            "temperature_2m_max": [)" + std::to_string(daily_max) + R"(],
            "temperature_2m_min": [)" + std::to_string(daily_min) + R"(]
        }
    })";
}

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

TEST_CASE("WeatherService::ParseCurrentConditions", "[service][weather_service]") {
    using namespace weather_cli;
    using Catch::Approx;

    SECTION("Parses a valid response into the correct fields") {
        auto cc = WeatherService::ParseCurrentConditions(ValidJson());

        REQUIRE(cc.temperature  == Approx(18.5).epsilon(0.01));
        REQUIRE(cc.feels_like   == Approx(16.2).epsilon(0.01));
        REQUIRE(cc.humidity     == 85);
        REQUIRE(cc.wind_speed   == Approx(26.1).epsilon(0.01));
        REQUIRE(cc.weather_code == 61);
        REQUIRE(cc.daily_max    == Approx(21.0).epsilon(0.01));
        REQUIRE(cc.daily_min    == Approx(12.5).epsilon(0.01));
    }

    SECTION("Parses a clear-sky (WMO 0) response") {
        auto cc = WeatherService::ParseCurrentConditions(
            ValidJson(32.0, 30.0, 20, 5.0, 0, 35.0, 24.0));

        REQUIRE(cc.weather_code == 0);
        REQUIRE(cc.temperature  == Approx(32.0).epsilon(0.01));
        REQUIRE(cc.daily_max    == Approx(35.0).epsilon(0.01));
    }

    SECTION("Parses a thunderstorm (WMO 95) response") {
        auto cc = WeatherService::ParseCurrentConditions(
            ValidJson(14.0, 11.0, 95, 60.0, 95, 16.0, 10.0));

        REQUIRE(cc.weather_code == 95);
        REQUIRE(cc.wind_speed   == Approx(60.0).epsilon(0.01));
    }

    SECTION("Throws on missing 'current' block") {
        const std::string json = R"({"daily": {"temperature_2m_max": [21.0], "temperature_2m_min": [12.5]}})";
        REQUIRE_THROWS_AS(
            WeatherService::ParseCurrentConditions(json),
            nlohmann::json::out_of_range);
    }

    SECTION("Throws on missing field inside 'daily'") {
        const std::string json = R"({
            "current": {
                "temperature_2m": 18.5,
                "apparent_temperature": 16.2,
                "relative_humidity_2m": 85,
                "wind_speed_10m": 26.1,
                "weather_code": 61
            },
            "daily": {
                "temperature_2m_max": [21.0]
            }
        })";
        REQUIRE_THROWS_AS(
            WeatherService::ParseCurrentConditions(json),
            nlohmann::json::out_of_range);
    }

    SECTION("Throws on completely malformed JSON") {
        REQUIRE_THROWS(
            WeatherService::ParseCurrentConditions("not json at all {{"));
    }
}
