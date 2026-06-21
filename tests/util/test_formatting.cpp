#include <catch2/catch_test_macros.hpp>
#include "util/formatting.hpp"

TEST_CASE("CelsiusToFahrenheit Conversion", "[util][formatting]") {
    using namespace weather_cli;

    REQUIRE(CelsiusToFahrenheit(0.0) == 32.0);
    REQUIRE(CelsiusToFahrenheit(10.0) == 50.0);
    REQUIRE(CelsiusToFahrenheit(-10.0) == 14.0);
    REQUIRE(CelsiusToFahrenheit(-40.0) == -40.0);
}

TEST_CASE("FormatTemperature Output formatting", "[util][formatting]") {
    using namespace weather_cli;

    SECTION("Celsius formatting outputs 1 decimal point and ends in °C") {
        REQUIRE(FormatTemperature(18.54, true) == "18.5°C");
        REQUIRE(FormatTemperature(0.0, true) == "0.0°C");
        REQUIRE(FormatTemperature(-5.19, true) == "-5.2°C");
    }

    SECTION("Fahrenheit formatting converts and formats to 1 decimal point and ends in °F") {
        REQUIRE(FormatTemperature(0.0, false) == "32.0°F");
        REQUIRE(FormatTemperature(10.0, false) == "50.0°F");
        REQUIRE(FormatTemperature(-10.0, false) == "14.0°F");
    }
}

TEST_CASE("MapWmoCodeToDescription Mapping", "[util][formatting]") {
    using namespace weather_cli;

    REQUIRE(MapWmoCodeToDescription(0) == "Clear sky");
    REQUIRE(MapWmoCodeToDescription(1) == "Partly cloudy");
    REQUIRE(MapWmoCodeToDescription(2) == "Partly cloudy");
    REQUIRE(MapWmoCodeToDescription(3) == "Partly cloudy");
    REQUIRE(MapWmoCodeToDescription(51) == "Drizzle");
    REQUIRE(MapWmoCodeToDescription(53) == "Drizzle");
    REQUIRE(MapWmoCodeToDescription(55) == "Drizzle");
    REQUIRE(MapWmoCodeToDescription(61) == "Rain");
    REQUIRE(MapWmoCodeToDescription(63) == "Rain");
    REQUIRE(MapWmoCodeToDescription(65) == "Rain");
    REQUIRE(MapWmoCodeToDescription(71) == "Snow fall");
    REQUIRE(MapWmoCodeToDescription(73) == "Snow fall");
    REQUIRE(MapWmoCodeToDescription(75) == "Snow fall");
    REQUIRE(MapWmoCodeToDescription(80) == "Rain showers");
    REQUIRE(MapWmoCodeToDescription(81) == "Rain showers");
    REQUIRE(MapWmoCodeToDescription(82) == "Rain showers");
    REQUIRE(MapWmoCodeToDescription(95) == "Thunderstorm");
    REQUIRE(MapWmoCodeToDescription(-1) == "Unknown Condition");
    REQUIRE(MapWmoCodeToDescription(100) == "Unknown Condition");
}
